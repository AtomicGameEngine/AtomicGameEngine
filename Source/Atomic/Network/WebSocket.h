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

#include "../Core/Mutex.h"
#include "../Container/ArrayPtr.h"
#include "../Container/List.h"
#include "../Container/RefCounted.h"
#include "../IO/Deserializer.h"

namespace Atomic
{

/// WebSocket connection state
enum WebSocketState
{
    WS_INITIALIZING = 0, // Establishing connection
    WS_ERROR,            // Invalid state
    WS_OPEN,             // WebSocket is open
    WS_CLOSING,          // WebSocket is being closed (and is probably still open)
    WS_CLOSED,           // WebSocket is closed or was disconnected
    WS_FAIL_TO_CONNECT   // WebSocket attempted to open, but the server refused
};

/// WebSocket internal state
struct WebSocketInternalState;

/// A WebSocket connection.
class WebSocket : public RefCounted
{
    REFCOUNTED(WebSocket)

public:
    /// Construct with parameters.
    WebSocket(const String& url);
    /// Destruct. Release the connection object.
    ~WebSocket();

    /// Return URL used in the request.
    const String& GetURL() const;

    /// Return error. Only non-empty in the error state.
    String GetError() const;
    /// Return connection state.
    WebSocketState GetState() const;
    /// Return the number of messages waiting.
    unsigned GetNumberOfMessages() const;

    /// Get the next message.
    String GetNextMessage();
    /// Send a message.
    void Send(String message);

    /// Disconnect the WebSocket.
    void Close();

    /// Attempt to reconnect the WebSocket.
    void OpenAgain();

    /// Return whether connection is in the open state.
    bool IsOpen() const { return GetState() == WS_OPEN; }

private:
    /// Internal state.
    WebSocketInternalState* is_;
};

}
