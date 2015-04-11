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

#ifndef SIMPLE_IPC_PIPE_WIN_H_
#define SIMPLE_IPC_PIPE_WIN_H_

#include "os_includes.h"
#include "ipc_constants.h"

class PipePair {
public:
  PipePair(bool inherit_fd2 = false);
  HANDLE fd1() const { return srv_; }
  HANDLE fd2() const { return cln_; }

  static HANDLE OpenPipeServer(const wchar_t* name, bool low_integrity = true);
  static HANDLE OpenPipeClient(const wchar_t* name, bool inherit, bool impersonate);

private:
  HANDLE srv_;
  HANDLE cln_;
};

class PipeWin {
public:
  PipeWin();
  ~PipeWin();

  bool OpenClient(HANDLE pipe);
  bool OpenServer(HANDLE pipe, bool connect = false);

  bool Write(const void* buf, size_t sz);
  bool Read(void* buf, size_t* sz);

  bool IsConnected() const { return INVALID_HANDLE_VALUE != pipe_; }

private:
  HANDLE pipe_;
};


class PipeTransport : public PipeWin {
public:
  static const size_t kBufferSz = 4096;

  size_t Send(const void* buf, size_t sz) {
    return Write(buf, sz) ? ipc::RcOK : ipc::RcErrTransportWrite;
  }

  char* Receive(size_t* size);

private:
  IPCCharVector buf_;
};

#endif  // SIMPLE_IPC_PIPE_WIN_H_
