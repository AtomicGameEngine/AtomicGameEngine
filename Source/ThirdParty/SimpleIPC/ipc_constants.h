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

#ifndef SIMPLE_IPC_CONSTANTS_H_
#define SIMPLE_IPC_CONSTANTS_H_

namespace ipc {

// No error is always 0. Positive values are reserved for the application
// while negative errors are reserved for the IPC library.

const size_t RcOK                   = 0;
const size_t RcErrEncoderClose      = static_cast<size_t>(-1);
const size_t RcErrEncoderBuffer     = static_cast<size_t>(-2);
const size_t RcErrEncoderType       = static_cast<size_t>(-3);
const size_t RcErrTransportWrite    = static_cast<size_t>(-4);
const size_t RcErrTransportRead     = static_cast<size_t>(-5);
const size_t RcErrTransportConnect  = static_cast<size_t>(-6);
const size_t RcErrDecoderFormat     = static_cast<size_t>(-7);
const size_t RcErrDecoderArgs       = static_cast<size_t>(-8);
const size_t RcErrNewTransport      = static_cast<size_t>(-9);
const size_t RcErrBadMessageId      = static_cast<size_t>(-10);

// For the return on obj.OnMsg() when calling Channel::Receive(obj) there
// are two critical values:
// - OnMsgLoopNext:  typically always returned by the server OnMsg()
//                   so it goes to process the next message
// - OnMsgAppErrorBase: typically always returned by the client OnMsg()
//                   so Receive() exits.
//
const size_t OnMsgLoopNext      = 0;  // Loop again and read another message.
const size_t OnMsgReady         = 1;  // Done receiving.
const size_t OnMsgAppErrorBase  = 2;  // Application specific errors start here.

// It is important to note that OnMsgLoopNext is 0 which is also RcOK. This
// enables the following server (broker) side pattern
//
//  size_t OnMsg(ChannelT* ch, param1, param2,...) {
//    ... operation here
//    return SendMsg(kRepyId, ch, result1, ..);
//  }


// Reserved messages ids. User messages should start at kMessagePrivLastId and
// above.
const int kMessagePrivNewTransport   = 1;
const int kMessagePrivControl        = 2;
const int kMessagePrivLastId         = 3;


}  // namespace ipc.

#endif // SIMPLE_IPC_CONSTANTS_H_

