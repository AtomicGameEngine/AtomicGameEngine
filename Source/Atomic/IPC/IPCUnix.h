
#ifndef ATOMIC_PLATFORM_WINDOWS

#pragma once

#include "../Core/Object.h"

namespace Atomic
{

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

  bool Send(const void* buf, size_t sz) {
    return Write(buf, sz);
  }

  char* Receive(size_t* size);

private:
  PODVector<char> buf_;
};

class IPCProcess : public Object
{
    OBJECT(IPCProcess)

public:

    IPCProcess(Context* context, int fd1, int fd2, int pid = -1);

    virtual ~IPCProcess();

    bool IsRunning();

    int fd1() const { return fd1_; }
    int fd2() const { return fd2_; }

    bool Launch(const String& command, const Vector<String>& args, const String& initialDirectory);

private:

    int pid_;
    int fd1_;
    int fd2_;
};


}


#endif
