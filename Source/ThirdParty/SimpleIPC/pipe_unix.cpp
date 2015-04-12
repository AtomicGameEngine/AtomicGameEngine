// Copyright (c) 2010 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "pipe_unix.h"

#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#define HANDLE_EINTR(x) ({ \
typeof(x) __eintr_result__; \
do { \
__eintr_result__ = x; \
} while (__eintr_result__ == -1 && errno == EINTR); \
__eintr_result__;\
})


namespace  {

bool SilenceSocket(int fd) {
  int nosigpipe = 1;
  // On OSX an attempt to read or write to a closed socket may generate a
  // SIGPIPE rather than returning -1.  setsockopt will shut this off.
  if (0 != setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE,
                      &nosigpipe, sizeof nosigpipe)) {
    return false;
  }
  return true;
}

size_t ReadFromFD(int fd, char* buffer, size_t bytes) {
  ssize_t bytes_read =
  HANDLE_EINTR(read(fd, buffer, bytes));
  if (bytes_read < 0) {
    return -1;
  }
  return bytes_read;
}

size_t WriteToFD(int fd, const char* data, size_t size) {
  // Allow for partial writes.
  ssize_t written_total = 0;
  for (ssize_t written_partial = 0; written_total < size; written_total += written_partial) {
    written_partial =
    HANDLE_EINTR(write(fd, data + written_total, size - written_total));
    if (written_partial < 0) {
      return -1;
    }
  }
  return written_total;
}

}  // namespace


PipePair::PipePair(bool unused) {
  fd_[0] = -1;
  fd_[1] = -1;
  
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd_) !=0) {
    return;
  }
};

PipeUnix::PipeUnix() : fd_(-1) {
}

bool PipeUnix::OpenClient(int fd) {
  if (!SilenceSocket(fd)) {
    return false;
  }
  fd_ = fd;
  return true;
}

bool PipeUnix::OpenServer(int fd) {
  if (!SilenceSocket(fd)) {
    return false;
  }  
  fd_ = fd;
  return true;
}


bool PipeUnix::Write(const void* buf, size_t sz) {
  if (sz == -1) {
    return false;
  }
  size_t written = WriteToFD(fd_, static_cast<const char*> (buf), sz);
  return (sz == written);
}

bool PipeUnix::Read(void* buf, size_t* sz) {
  size_t read = ReadFromFD(fd_, static_cast<char*> (buf), *sz);
  if (read == -1) {
    return false;
  }
  *sz = read;
  return true;
}


char* PipeTransport::Receive(size_t* size) {
  if (buf_.size() < kBufferSz) {
    buf_.resize(kBufferSz);
  }
  
  *size = kBufferSz;
  if (!Read(&buf_[0], size)) {
    return NULL;
  }
  return &buf_[0];
}


