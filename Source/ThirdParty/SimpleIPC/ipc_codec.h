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

#ifndef SIMPLE_IPC_CODEC_H_
#define SIMPLE_IPC_CODEC_H_

#include "os_includes.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// This file contains the default encoder & decoder for IPC. It does no compression and every
// encoded element is aligned to the machine word size. For a 32 bit machine, this is how the
// format on the final buffer looks like:
//
// bytes what
// 0     start of header mark
// 4     msg id
// 8     element count (1 to N)
// 12    data count (in 4 byte units
// 16    first element tag
// 20    second element tag
// +4    .......
//       last element type
//       start of data mark     
//       first element value
//       second element value
//       .......
//       end of data mark
//
// As you can see the element tags are not interleaved with the element value, all the tags
// are within the header, and all values follow afterwards.
//
// This code does not assume any knowledge of the Channel type. For example is unaware of WireType
// so it takes a generic |tag| that in the case of using it with the standard ipc::Channel they
// would be ipc::TYPE_XXXXX. However, arrays (bytes and strings) are treated differently in which
// the tag is actually 'enhanced' with an extra bit in the form of ENC_STRN08 or ENC_STRN16.

namespace ipc {

class Encoder {
public:
  enum {
    ENC_HEADER = 0x4d4f524b,
    ENC_STARTD = 0x4b524f4d,
    ENC_ENDDAT = 0x474e4142,
    ENC_STRN08 = 1<<30,
    ENC_STRN16 = 1<<31
  };

  Encoder() : index_(-1) {}

  bool Open(int count) {
    data_.clear();
    data_.reserve(count * 5);
    data_.resize(count + 5);
    index_ = -1;
    SetHeaderNext(ENC_HEADER);  // 0
    SetHeaderNext(0);           // 1
    SetHeaderNext(count);       // 2
    SetHeaderNext(0);           // 3
    return true;
  }

  bool Close() {
    SetHeaderNext(ENC_STARTD);
    PushBack(ENC_ENDDAT);
    SetDataSizeHeader();
    return true;
  }
  
  bool OnWord(void* bits, int tag) {
    SetHeaderNext(tag);
    data_.push_back(bits);
    return true;
  };

  bool OnString8(const IPCString& s, int tag) {
    SetHeaderNext(tag | ENC_STRN08);
    PushBack(s.size());
    if (s.size()) AddStr(s);
    return true;
  }

  bool OnString16(const IPCWString& s, int tag) {
    SetHeaderNext(tag | ENC_STRN16);
    PushBack(s.size());
    if (s.size()) AddStr(s);
    return true;
  }

  bool OnUnixFd(int /*fd*/, int tag) {
    SetHeaderNext(tag);
    // $$ implement
    return true;
  }

  bool OnWinHandle(void* /*handle*/, int tag) {
    SetHeaderNext(tag);
    // $$ implement
    return true;
  }

  const void* GetBuffer(size_t* sz) {
    *sz = data_.size() * sizeof(void*);
    return &data_[0];
  }

  void SetMsgId(int id) {
    data_[1] = reinterpret_cast<void*>(id);
  }

private:

  void SetHeaderNext(int v) {
    data_[++index_] = reinterpret_cast<void*>(v);
  }

  void SetDataSizeHeader() {
    data_[3] = reinterpret_cast<void*>(data_.size());
  }

  void PushBack(int v) {
    data_.push_back(reinterpret_cast<void*>(v));
  }

  void PushBack(unsigned int v) {
    data_.push_back(reinterpret_cast<void*>(v));
  }

  template <typename StringT> 
  void AddStr(const StringT& s ) {
    const int times = sizeof(IPCVoidPtrVector::value_type) / sizeof(s[0]);
    size_t it = 0;
    do {
      unsigned int v = 0;
      for (int ix = 0; ix != times; ++ix) {
        if (it == s.size())
          break;
        v |= PackChar(s[it], ix);
        ++it;
      }
      PushBack(v);
    } while (it != s.size());
  }

  unsigned int PackChar(char c, int offset) const {
    return static_cast<unsigned char>(c) << (offset * 8);
  }

#if 0
  unsigned int PackChar(wchar_t c, int offset) const {
    unsigned int u = (static_cast<unsigned char>(c) << 8) | static_cast<unsigned char>(c >> 8)
    return  u << offset * 16;
  }
#endif

  unsigned int PackChar(wchar_t c, int offset) const {
    const char* t = reinterpret_cast<const char*>(&c);
    return  (PackChar(t[0], 0) | PackChar(t[1], 1)) << (offset * 16);
  }

  IPCVoidPtrVector data_;
  int index_;
};


template <typename HandlerT>
class Decoder {
public:
  Decoder(HandlerT* handler) : handler_(handler) {
    Reset();
  }

  bool OnData(const char* buff, size_t sz) {
    if (buff) {
      data_.insert(data_.end(), buff, buff + sz);
    } else if (!data_.size()) {
      return true;
    }
    if (data_.size() % sizeof(void*) == 0) {
      return (RunDecoder() == DEC_MOREDATA);
    }
    return true;
  }

  bool Success() { return state_ == DEC_S_DONE; }

  bool NeedsMoreData() const {
    return (data_.size() == 0) || (res_ == DEC_MOREDATA); 
  }

  void Reset() {
    state_ = DEC_S_START;
    e_count_ = -1;
    d_count_ = static_cast<size_t>(-1);
    next_char_ = 0;
    res_ = DEC_NONE;
  }

private:
  // States of the decoder state machine, for a single message
  // they are basically traveled from the first to the last and
  // there is no state that means 'error'. When an error happens
  // the state machine stops at the state that caused it.
  enum State {
    DEC_S_START,
    DEC_S_HEADSZ,
    DEC_S_EDATA,
    DEC_S_STOP,
    DEC_S_DONE
  };

  // The possible results of running one step on the state machine.
  // DEC_LOOPAGAIN causes the state machine to be spin up one more
  // time while the others cause it to return to the caller so
  // it can provide more data, handle a decoding error or process
  // a ready message.
  enum Result {
    DEC_NONE,
    DEC_LOOPAGAIN,
    DEC_MOREDATA,
    DEC_DONE,
    DEC_ERROR
  };

  Result RunDecoder() {
    do {
      res_ = DecodeStep();
    } while (DEC_LOOPAGAIN == res_);
    return res_;
  }

  Result DecodeStep() {
    switch(state_) {
      case DEC_S_START: return StateStart();
      case DEC_S_HEADSZ: return StateHeader();
      case DEC_S_EDATA: return StateData();
      case DEC_S_STOP: return StateDone();
      default: break;
    }
    return DEC_ERROR;
  }

  Result StateStart() {
    // We need at least the first 4 ints.
    if (!HasEnoughUnProcessed(4))
      return DEC_MOREDATA;
    int i0 = ReadNextInt();
    if (Encoder::ENC_HEADER != i0)
      return DEC_ERROR;
    int msg_id = ReadNextInt();
    if (msg_id < 0)
      return DEC_ERROR;
    e_count_ = ReadNextInt();
    if ((e_count_ < 1) || (e_count_ > 100))
      return DEC_ERROR;
    d_count_ = ReadNextInt();
    if ((d_count_ < 5) || (d_count_ > (8 * 1024 * 1024)))
      return DEC_ERROR;
    // Done with the key header piece.
    if (!handler_->OnMessageStart(msg_id, e_count_))
      return DEC_ERROR;
    // The handler is willing to accept the message.
    d_count_ -= 4;
    state_ = DEC_S_HEADSZ;
    if (HasEnoughUnProcessed(1))
      return DEC_LOOPAGAIN;
    return DEC_MOREDATA;
  }

  Result StateHeader() {
    if (!HasEnoughUnProcessed(e_count_ + 1))
      return DEC_MOREDATA;
    //items_.reserve(e_count_);
    for (int ix = 0; ix != e_count_; ++ix) {
      items_.push_back(ReadNextInt());
    }
    int i0 = ReadNextInt();
    if (i0 != Encoder::ENC_STARTD)
      return DEC_ERROR;
    // Done with all the header
    d_count_ -= e_count_ + 1;
    if (!items_.size()) {
      // That's it, no data.
      state_ = DEC_S_STOP;
      if (HasEnoughUnProcessed(1))
        return DEC_LOOPAGAIN;
      return DEC_MOREDATA;
    }
    // We got data to process. Each datum, even null strings
    // take at least 4 bytes.
    if (d_count_ < items_.size())
      return DEC_ERROR;

    state_ = DEC_S_EDATA;
    if (HasEnoughUnProcessed(1))
      return DEC_LOOPAGAIN;
    return DEC_MOREDATA;
  }

  Result StateData() {
    if (!HasEnoughUnProcessed(d_count_))
      return DEC_MOREDATA;
    size_t ix = 0;
    do {
        int tag = items_[ix];
        if (tag & Encoder::ENC_STRN08) {
          tag &= ~Encoder::ENC_STRN08;
          ReadNextStr8(tag);
        } else if (tag & Encoder::ENC_STRN16) {
          tag &= ~Encoder::ENC_STRN16;
          ReadNextStr16(tag);
        } else {
          --d_count_;
          if (!handler_->OnWord(ReadNextVoidPtr(), tag)) {
            return DEC_ERROR;
          }
        }
        ++ix;
        if (items_.size() == ix) {
          items_.clear();
          state_ = DEC_S_STOP;
          if (HasEnoughUnProcessed(1))
            return DEC_LOOPAGAIN;
          return DEC_MOREDATA;
        }
    } while (HasEnoughUnProcessed(1));
    return DEC_MOREDATA;
  }

  Result StateDone() {
    if (!HasEnoughUnProcessed(1))
      return DEC_MOREDATA;
    --d_count_;
    int it0 = ReadNextInt();
    if (Encoder::ENC_ENDDAT != it0)
      return DEC_ERROR;

    data_.erase(data_.begin(), data_.begin() + next_char_);
    state_ = DEC_S_DONE;
    return DEC_DONE;
  }

  int ReadNextInt() {
    int v  = *reinterpret_cast<int*>(&data_[next_char_]);
    next_char_ += sizeof(void*);
    return v;
  }

  bool ReadNextStr8(int tag) {
    size_t str_sz = ReadNextInt();
    size_t sz_rounded = RoundUpToNextVoidPtr(str_sz);
    if(!HasEnoughUnProcessed(sz_rounded))
      return false;
    const char* beg = &data_[next_char_];
    IPCString str;
    str.assign(beg, str_sz);
    next_char_ += sz_rounded * sizeof(void*);
    handler_->OnString8(str, tag);
    return true;
  }

  bool ReadNextStr16(int tag) {
    size_t str_sz = ReadNextInt();
    size_t byte_sz = str_sz * sizeof(wchar_t);
    size_t sz_rounded = RoundUpToNextVoidPtr(byte_sz);
    if(!HasEnoughUnProcessed(sz_rounded))
      return false;
    const wchar_t* beg = reinterpret_cast<wchar_t*>(&data_[next_char_]);
    IPCWString str;
    str.assign(beg, str_sz);
    next_char_ += sz_rounded * sizeof(void*);
    handler_->OnString16(str, tag);
    return true;
  }

  void* ReadNextVoidPtr() {
    void* v = &data_[next_char_];
    next_char_ += sizeof(void*);
    return v;
  }

  char ReadNextChar() {
    return data_[next_char_++];
  }

  bool HasEnoughUnProcessed(int ints) {
    return ((data_.size() - next_char_) >= (ints * sizeof(void*)));
  }

  int RoundUpToNextVoidPtr(int sz) {
    return (sz + (sizeof(void*)-1))/sizeof(void*);
  }        

  HandlerT* handler_;

  IPCCharVector data_;
  IPCIntVector items_;

  State state_;
  int e_count_;
  size_t d_count_;
  int next_char_;
  Result res_;
};


}  // namespace ipc.

#endif  // SIMPLE_IPC_CODEC_H_
