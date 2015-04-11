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

#include "pipe_win.h"
#include <Sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>

namespace {
const wchar_t kPipePrefix[] = L"\\\\.\\pipe\\";
const int kPipeBufferSz = 4 * 1024;
}  // namespace

bool checkIntegritySupport() {
  OSVERSIONINFO osvi;

  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);

  return osvi.dwMajorVersion > 5;
}

LONG g_pipe_seq = 0;

HANDLE PipePair::OpenPipeServer(const wchar_t* name, bool low_integrity) {
  SECURITY_ATTRIBUTES sa = {0};
  SECURITY_ATTRIBUTES *psa = 0;

  static const bool is_integrity_supported = checkIntegritySupport();

  if (is_integrity_supported && low_integrity) {
    psa = &sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptor(
        TEXT("S:(ML;;NWNR;;;LW)"), SDDL_REVISION_1, &sa.lpSecurityDescriptor, NULL)) 
      return INVALID_HANDLE_VALUE;
  }

  IPCWString pipename(kPipePrefix);
  pipename.append(name);
  return ::CreateNamedPipeW(pipename.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
                            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                            1, kPipeBufferSz, kPipeBufferSz, 200, psa);
}

HANDLE PipePair::OpenPipeClient(const wchar_t* name, bool inherit, bool impersonate) {
  IPCWString pipename(kPipePrefix);
  pipename.append(name);

  SECURITY_ATTRIBUTES sa = {sizeof(sa), NULL, inherit ? TRUE : FALSE};
  for (;;) {
    DWORD attributes = impersonate ? 0 : SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION;
    HANDLE pipe = ::CreateFileW(pipename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, &sa,
                                OPEN_EXISTING, attributes, NULL);
    if (INVALID_HANDLE_VALUE == pipe) {
      if (ERROR_PIPE_BUSY != ::GetLastError()) {
        return pipe;
      }
      // wait and retry.
      ::Sleep(25);
    } else {
      // success.
      return pipe;
    }
  }
}


PipePair::PipePair(bool inherit_fd2) : srv_(NULL), cln_(NULL) {
  // Come up with a reasonable unique name.
  const wchar_t kPipePattern[] = L"ko.%x.%x.%x";
  wchar_t name[8*3 + sizeof(kPipePattern)];
  ::wsprintfW(name, kPipePattern, ::GetCurrentProcessId(), ::GetTickCount(), 
              ::InterlockedIncrement(&g_pipe_seq));
  HANDLE server = OpenPipeServer(name);
  if (INVALID_HANDLE_VALUE == server) {
    return;
  }
  // Don't allow client impersonation.
  HANDLE client = OpenPipeClient(name, inherit_fd2, false);
  if (INVALID_HANDLE_VALUE == client) {
    ::CloseHandle(server);
    return;
  }
  if (!::ConnectNamedPipe(server, NULL)) {
    if (ERROR_PIPE_CONNECTED != ::GetLastError()) {
      ::CloseHandle(server);
      ::CloseHandle(client);
    return;
    }
  }

  srv_ = server;
  cln_ = client;
}


PipeWin::PipeWin() : pipe_(INVALID_HANDLE_VALUE) {
}

PipeWin::~PipeWin() {
  if (pipe_ != INVALID_HANDLE_VALUE) {
    ::DisconnectNamedPipe(pipe_);  // $$$ disconect is valid on the server side.
    ::CloseHandle(pipe_);
  }
}

bool PipeWin::OpenClient(HANDLE pipe) {
  pipe_ = pipe;  
  return true;
}

bool PipeWin::OpenServer(HANDLE pipe, bool connect) {
  pipe_ = pipe;

  if (connect) {
    if (!::ConnectNamedPipe(pipe, NULL)) {
      if (ERROR_PIPE_CONNECTED != ::GetLastError()) {
        return false;
      }
    }
  }
  return true;
}

bool PipeWin::Write(const void* buf, size_t sz) {
  DWORD written = 0;
  return (TRUE == ::WriteFile(pipe_, buf, sz, &written, NULL));
}

bool PipeWin::Read(void* buf, size_t* sz) {
  return (TRUE == ::ReadFile(pipe_, buf, *sz, reinterpret_cast<DWORD*>(sz), NULL));
}


char* PipeTransport::Receive(size_t* size) {
  if (buf_.size() < kBufferSz)
    buf_.resize(kBufferSz);

  *size = kBufferSz;
  if (!Read(&buf_[0], size))
    return NULL;
  return &buf_[0];
}
