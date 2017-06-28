/**
Lightweight profiler library for c++
Copyright(C) 2016-2017  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
	* MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights 
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
	of the Software, and to permit persons to whom the Software is furnished 
	to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all 
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
	USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
	You may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

**/

#ifndef EASY_NET_H
#define EASY_NET_H

#include <stdint.h>

namespace profiler {
namespace net {

const uint32_t EASY_MESSAGE_SIGN = 20160909;

#pragma pack(push,1)

enum MessageType : uint8_t
{
    MESSAGE_TYPE_ZERO = 0,

    MESSAGE_TYPE_REQUEST_START_CAPTURE,
    MESSAGE_TYPE_REPLY_START_CAPTURING,
    MESSAGE_TYPE_REQUEST_STOP_CAPTURE,

    MESSAGE_TYPE_REPLY_BLOCKS,
    MESSAGE_TYPE_REPLY_BLOCKS_END,

    MESSAGE_TYPE_ACCEPTED_CONNECTION,

    MESSAGE_TYPE_REQUEST_BLOCKS_DESCRIPTION,
    MESSAGE_TYPE_REPLY_BLOCKS_DESCRIPTION,
    MESSAGE_TYPE_REPLY_BLOCKS_DESCRIPTION_END,

    MESSAGE_TYPE_EDIT_BLOCK_STATUS,

    MESSAGE_TYPE_EVENT_TRACING_STATUS,
    MESSAGE_TYPE_EVENT_TRACING_PRIORITY,
    MESSAGE_TYPE_CHECK_CONNECTION,

    MESSAGE_TYPE_REQUEST_MAIN_FRAME_TIME_MAX_AVG_US,
    MESSAGE_TYPE_REPLY_MAIN_FRAME_TIME_MAX_AVG_US,
};

struct Message
{
    uint32_t  magic_number = EASY_MESSAGE_SIGN;
    MessageType type = MESSAGE_TYPE_ZERO;

    bool isEasyNetMessage() const
    {
        return EASY_MESSAGE_SIGN == magic_number;
    }

    Message() = default;
    Message(MessageType _t):type(_t){}
};

struct DataMessage : public Message {
    uint32_t size = 0; // bytes
    DataMessage(MessageType _t = MESSAGE_TYPE_REPLY_BLOCKS) : Message(_t) {}
    DataMessage(uint32_t _s, MessageType _t = MESSAGE_TYPE_REPLY_BLOCKS) : Message(_t), size(_s) {}
    const char* data() const { return reinterpret_cast<const char*>(this) + sizeof(DataMessage); }
};

struct BlockStatusMessage : public Message {
    uint32_t    id;
    uint8_t status;
    BlockStatusMessage(uint32_t _id, uint8_t _status) : Message(MESSAGE_TYPE_EDIT_BLOCK_STATUS), id(_id), status(_status) { }
private:
    BlockStatusMessage() = delete;
};

struct EasyProfilerStatus : public Message
{
    bool         isProfilerEnabled;
    bool     isEventTracingEnabled;
    bool isLowPriorityEventTracing;

    EasyProfilerStatus(bool _enabled, bool _ETenabled, bool _ETlowp)
        : Message(MESSAGE_TYPE_ACCEPTED_CONNECTION)
        , isProfilerEnabled(_enabled)
        , isEventTracingEnabled(_ETenabled)
        , isLowPriorityEventTracing(_ETlowp)
    {
    }

private:

    EasyProfilerStatus() = delete;
};

struct BoolMessage : public Message {
    bool flag = false;
    BoolMessage(MessageType _t, bool _flag = false) : Message(_t), flag(_flag) { }
    BoolMessage() = default;
};

struct TimestampMessage : public Message {
    uint32_t maxValue = 0;
    uint32_t avgValue = 0;
    TimestampMessage(MessageType _t, uint32_t _maxValue, uint32_t _avgValue) : Message(_t), maxValue(_maxValue), avgValue(_avgValue) { }
    TimestampMessage() = default;
};

#pragma pack(pop)

}//net

}//profiler

#endif // EASY_NET_H
