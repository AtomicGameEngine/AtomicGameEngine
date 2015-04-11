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

#ifndef SIMPLE_IPC_CHANNEL_H_
#define SIMPLE_IPC_CHANNEL_H_

#include "ipc_constants.h"
#include "ipc_utils.h"
#include "ipc_wire_types.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// The channel class is a controler/coordinator between the different IPC actors:
// 1- the transport (like a pipe)
// 2- the message encoder
// 3- the message decoder
// 4- the message
//
// The unit of communication of a channel is the message. The channel does not differenciate
// sender or receiver and assumes that the transport is bi-directional.
// 
// For outgoing messages, a message is just defined as an array of WireType pointers along with
// a message id. The encoder and decoder are loosely coupled with the message and it is the job
// of the channel to interface them.
//
// Sending Requirements
//  Encoder should implement:
//    bool Open(int n_args)
//    bool Close()
//    void SetMsgId(int msg_id)
//    bool OnWord(void* bits, int tag)
//    bool OnString8(const string& s, int tag)
//    bool OnString16(const wstring& s, int tag)
//    bool OnUnixFd(int fd, int tag)
//    bool OnWinHandle(void* handle, int tag)
//    const void* GetBuffer(size_t* sz)
//
// Receiving Requirements
//  Decoder<Handler> should implement:
//    bool OnData(const char* buff, size_t sz)
//    bool Success()
//  Decoder<Handler> should call:
//    bool Handler::OnMessageStart(int id, int n_args)
//    bool Handler::OnWord(const void* bits, int type_id)
//    bool Handler::OnString8(string& str, int type_id) 
//    bool Handler::OnString16(wstring& str, int type_id)
//

namespace ipc {

template <class TransportT, class EncoderT, template <class> class DecoderT>
class Channel {
 public:
  static const size_t kMaxNumArgs = 10;
  Channel(TransportT* transport) : transport_(transport), last_msg_id_(-1) {}

  // This is the last message that was received. Or at least the header was
  // correct so we could extract the message id.
  int LastRecvMsgId() const { return last_msg_id_; }

  // Sends the message (|args| + msg_id) to the other end of the connected
  // |transport| passed to the constructor. This call can block or not depending
  // on the transport implementation.
  size_t Send(int msg_id, const WireType* const args[], int n_args)  {
    EncoderT encoder;
    encoder.Open(n_args);
    for (int ix = 0; ix != n_args; ++ix) {
      if (!AddMsgElement(&encoder, *args[ix]))
        return RcErrEncoderType;
    }

    encoder.SetMsgId(msg_id);
    if (!encoder.Close())
      return RcErrEncoderClose;

    size_t size;
    const void* buf = encoder.GetBuffer(&size);
    if (!buf)
      return RcErrEncoderBuffer;
    return transport_->Send(buf, size);
  }

  // Blocking wait for a message to arrive to from the other end of the
  // |transport| passed in the constructor. If a valid message is received
  // the function calls |top_dispatch| and then returns with the return
  // value of |top_dispatch|.
  //
  // The DispatchT class is required to implement two functions:
  // 1- Tm* MsgHandler(int msg_id);
  //    The return value is a pointer to an object of a type that can handle |msg_id|.
  //    This class must implement:
  //    size_t Tm::OnMsgIn(int msg_id,
  //                       ChannelT* ch,
  //                       const WireType* const args[],
  //                       int count);
  //
  // 2- void* OnNewTransport();
  //    Returns the handle to a new transport. It also means to
  //    create a new thread or process to handle messages over that
  //    transport. Return NULL if this feature is not supported.
  //
  // Depending on the value returned by the message handler Tm::OnMsgIn(), this function
  // loops again (if return is 0) or finishes (return non-zero), so by necessity
  // this function never returns 0.
  //
  // The costume is to use ipc::OnMsgLoopNext (0) to loop and ipc::OnMsgReady (1)
  // to terminate with no error condtion. This is desirable but not necessary.
  //
  template <class DispatchT>
  size_t Receive(DispatchT* top_dispatch) {

    RxHandler handler;
    DecoderT<RxHandler> decoder(&handler);

    // There are two do/while nested loops. The inner one runs until a full message
    // has been decoded and the outer one runs until a dispatcher returns anything
    // but a 0. The inner loop has two modes, in one it requires more external data
    // and in the other it can keep processing what has been read so far. They are
    // required to handle the case of reading less than a full message and when
    // reading more than one message.
    size_t retv = 0;
    do {
      size_t received = 0;
       const char* buf = NULL;
      do {
        if (decoder.NeedsMoreData()) {
          buf = transport_->Receive(&received);
          if (!buf) {
            // read failed.
            return RcErrTransportRead;
          }
        } else {
          buf = NULL;
        }
      } while (decoder.OnData(buf, received));

      last_msg_id_ = handler.MsgId();

      if(!decoder.Success())
        return RcErrDecoderFormat;

      size_t np = handler.GetArgCount();
      if (np > kMaxNumArgs)
        return RcErrDecoderArgs;

      const WireType* args[kMaxNumArgs];
      for (size_t ix = 0; ix != np; ++ix) {
        args[ix] = &handler.GetArg(ix);
      }

      if ((handler.MsgId() == kMessagePrivNewTransport) &&
          (np == 1) && (args[0]->GetAsBits() == NULL)) {
        // Got special rpc to create a new transport. On the receiving side we handle it entirely
        // here by calling OnNewTransport and then sending the reply, but on the sending side it
        // is handled by a NewTransportHandler object so it actually uses top_dispatch->MsgHandler().
        void* handle = top_dispatch->OnNewTransport();
        retv = handle ? SendNewTransportMsg(handle) : ipc::OnMsgLoopNext;
      } else {
        // Got one regular message. Now dispatch it.
        retv = top_dispatch->MsgHandler(handler.MsgId())->OnMsgIn(handler.MsgId(), this, args, np);
      }

      handler.Clear();
      decoder.Reset();
    } while(ipc::OnMsgLoopNext == retv);

    return retv;
  }

  // Issues an rpc to the remote side, usually the server to get a new transport identifier, it is
  // some OS-dependent value that can be used to create a pipe or domain socket. It implies that
  // somehow the remote side will spin some machinery to answer messages sent this way.
  void* InitNewTransport() {
    size_t rc = SendNewTransportMsg(NULL);
    if (rc)
      return NULL;
    NewTransportHandler handler;
    rc = Receive(&handler);
    if (rc != ipc::OnMsgReady)
      return NULL;
    return handler.Handle();
  }

  // This class is using during receiving as the callback handler for the decoder.
  // Its function is to receive each decoded type and transform it into WireType objects.
  //
  // This class would be private except that we have unit tests that use it as
  // convenience. Treat it as private though.
  class RxHandler {
   public:
    RxHandler() : msg_id_(-1) {} 

    // Called when a valid message preamble is received.
    bool OnMessageStart(int id, int n_args) {
      msg_id_ = id;
      if (n_args > kMaxNumArgs)
        return false;
      return true;
    }

    // Handles the word-sized 'value' decoded types.
    bool OnWord(const void* bits, int type_id) {
      switch (type_id) {
        case ipc::TYPE_INT32:
          list_.push_back(WireType(*reinterpret_cast<const int*>(bits)));
          break;
        case ipc::TYPE_UINT32:
          list_.push_back(WireType(*reinterpret_cast<const unsigned int*>(bits)));
          break;
        case ipc::TYPE_LONG32:
          list_.push_back(WireType(*reinterpret_cast<const long*>(bits)));
          break;
        case ipc::TYPE_ULONG32:
          list_.push_back(WireType(*reinterpret_cast<const unsigned long*>(bits)));
          break;
        case ipc::TYPE_CHAR8:
          list_.push_back(WireType(*reinterpret_cast<const char*>(bits)));
          break;
        case ipc::TYPE_CHAR16:
          list_.push_back(WireType(*reinterpret_cast<const wchar_t*>(bits)));
          break;
        case ipc::TYPE_VOIDPTR:
          list_.push_back(WireType(reinterpret_cast<const void*>(
              *reinterpret_cast<const unsigned long*>(bits))));
          break;
        case ipc::TYPE_NULLSTRING8:
          list_.push_back(WireType(static_cast<char*>(NULL)));
          break;
        case ipc::TYPE_NULLSTRING16:
          list_.push_back(WireType(static_cast<wchar_t*>(NULL)));
          break;
        case ipc::TYPE_NULLBARRAY:
          list_.push_back(WireType(ipc::ByteArray(0, NULL)));
          break;
        default:
          return false;
      }
      return true;
    }

    // Handles the byte-sized arrays.
    bool OnString8(IPCString& str, int type_id) {
      switch (type_id) {
        case ipc::TYPE_STRING8:
          list_.push_back(WireType(str.c_str()));
          break;
        case ipc::TYPE_BARRAY:
          list_.push_back(WireType(ByteArray(str.size(), str.c_str())));
          break;
        default: 
          return false;
      }
      return true;
    }

    // Handles the wchar-sized arrays.
    bool OnString16(IPCWString& str, int type_id) {
      switch (type_id) {
        case ipc::TYPE_STRING16:
          list_.push_back(WireType(str.c_str()));
          break;
        default: 
          return false;
      }
      return true;
    }

    int MsgId() const { return msg_id_; }
    
    const WireType& GetArg(size_t ix) {
      return list_[ix];
    }

    size_t GetArgCount() const { return list_.size(); }

    void Clear() {
      list_.clear();
      msg_id_ = -1;
    }

  private:
    typedef FixedArray<WireType, (kMaxNumArgs + 1)> RxList;
    RxList list_;
    int msg_id_;
  };

private:
  // Class to handle the client side of the internal kMessagePrivNewTransport message
  // which carries a transport id, usually a handle value on windows.
  class NewTransportHandler {
  public:
    NewTransportHandler() : t_handle_(NULL) {}

    NewTransportHandler* MsgHandler(int) {
      return this;
    }

    size_t OnMsgIn(int msg_id, Channel*, const WireType* const args[], int count) {
      if ((count != 1) && (msg_id != kMessagePrivNewTransport))
        return RcErrNewTransport;
      t_handle_ = args[0]->GetAsBits();
      return ipc::OnMsgReady;
    }

    void* OnNewTransport() { return NULL; }
    void* Handle() const { return t_handle_; }
  
  private:
    void* t_handle_;
  };

  size_t SendNewTransportMsg(void* handle) {
    WireType wt(handle);
    const WireType* const arg[] = { &wt };
    return Send(kMessagePrivNewTransport, arg, 1);
  }

  // Uses |EncoderT| to encode one message element in the outgoing buffer.
  bool AddMsgElement(EncoderT* encoder, const WireType& wtype) {
    switch (wtype.Id()) {
      case ipc::TYPE_NONE:
        return false;

      case ipc::TYPE_INT32:
      case ipc::TYPE_UINT32:
      case ipc::TYPE_CHAR8:
      case ipc::TYPE_CHAR16:
      case ipc::TYPE_LONG32:
      case ipc::TYPE_ULONG32:
      case ipc::TYPE_VOIDPTR:
          return encoder->OnWord(wtype.GetAsBits(), wtype.Id());

      case ipc::TYPE_STRING8:
      case ipc::TYPE_BARRAY: {
          IPCString ctemp;
          wtype.GetString8(&ctemp);
          return encoder->OnString8(ctemp, wtype.Id());
        }

      case ipc::TYPE_STRING16: {
          IPCWString wtemp;
          wtype.GetString16(&wtemp);
          return encoder->OnString16(wtemp, wtype.Id());
        }

      case ipc::TYPE_NULLSTRING8:
      case ipc::TYPE_NULLSTRING16:
      case ipc::TYPE_NULLBARRAY:
        return encoder->OnWord(wtype.GetAsBits(), wtype.Id());

      default:
        return false;
    }
  }

  TransportT* transport_;
  int last_msg_id_;
};

}  // namespace ipc.

#endif // SIMPLE_IPC_CHANNEL_H_

