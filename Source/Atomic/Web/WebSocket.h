//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Core/Object.h"
#include "../Container/Str.h"
#include "../Container/Vector.h"

#include <memory>

namespace asio
{
class io_service;
}

namespace Atomic
{

/// WebSocket connection state
enum WebSocketState
{
    WS_CONNECTING = 0,   // Establishing connection
    WS_OPEN,             // WebSocket is open
    WS_CLOSING,          // WebSocket is being closed (and is probably still open)
    WS_CLOSED,           // WebSocket is closed or was disconnected
    WS_INVALID,          // Invalid state
    WS_FAIL_TO_CONNECT   // WebSocket attempted to open, but the server refused
};

enum WebSocketMessageType
{
    WSMT_CONTINUATION = 0x0,
    WSMT_TEXT = 0x1,
    WSMT_BINARY = 0x2,
    WSMT_RSV3 = 0x3,
    WSMT_RSV4 = 0x4,
    WSMT_RSV5 = 0x5,
    WSMT_RSV6 = 0x6,
    WSMT_RSV7 = 0x7,
    WSMT_CLOSE = 0x8,
    WSMT_PING = 0x9,
    WSMT_PONG = 0xA,
    WSMT_CONTROL_RSVB = 0xB,
    WSMT_CONTROL_RSVC = 0xC,
    WSMT_CONTROL_RSVD = 0xD,
    WSMT_CONTROL_RSVE = 0xE,
    WSMT_CONTROL_RSVF = 0xF
};

struct WebSocketInternalState;

/// A WebSocket connection.
class WebSocket : public Object
{
    friend class Web;

    ATOMIC_OBJECT(WebSocket, Object)

public:
    /// Construct with parameters.
    WebSocket(Context* context, const String& url);
    /// Destruct. Release the connection object.
    ~WebSocket();

    /// Return URL used in the request.
    const String& GetURL() const;

    /// Return error. Only non-empty in the error state.
    String GetError() const;
    /// Return connection state.
    WebSocketState GetState() const;

    /// Send a message.
    void Send(const String& message);

    /// Send a binary message.
    void SendBinary(const PODVector<unsigned char>& message);

    /// Disconnect the WebSocket.
    void Close();

    /// Attempt to reconnect the WebSocket.
    void OpenAgain();

    /// Return whether connection is in the open state.
    bool IsOpen() const { return GetState() == WS_OPEN; }

private:
    void setup(asio::io_service *service);
    std::shared_ptr<WebSocketInternalState> is_;
};

}
