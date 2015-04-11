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

#ifndef SIMPLE_IPC_PIPE_UNIX_H_
#define SIMPLE_IPC_PIPE_UNIX_H_


#include "os_includes.h"
#include "ipc_constants.h"

class PipePair {
public:
  PipePair();
  
  int fd1() const { return fd_[0]; }
  int fd2() const { return fd_[1]; }
  
private:
  int fd_[2];
};


class PipeUnix {
public:
  PipeUnix();

  bool OpenClient(int fd);
  bool OpenServer(int fd);

  bool Write(const void* buf, size_t sz);
  bool Read(void* buf, size_t* sz);

  bool IsConnected() const { return fd_ != -1; }

private:
  int fd_;
};


class PipeTransport : public PipeUnix {
public:
  static const size_t kBufferSz = 4096;
  
  size_t Send(const void* buf, size_t sz) {
    return Write(buf, sz) ? ipc::RcOK : ipc::RcErrTransportWrite;
  }
  
  char* Receive(size_t* size);

private:
  IPCCharVector buf_;
};


#endif  // SIMPLE_IPC_PIPE_UNIX_H_
