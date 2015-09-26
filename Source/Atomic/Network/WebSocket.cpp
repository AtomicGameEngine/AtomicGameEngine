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

#include "../Precompiled.h"

#include "../Core/Profiler.h"
#include "../IO/Log.h"
#include "../Network/WebSocket.h"

#ifdef EMSCRIPTEN

#include "../DebugNew.h"

// Add code to use an HTML5 style WebSocket here.

#else

#define _WEBSOCKETPP_NOEXCEPT_TOKEN_
#define _WEBSOCKETPP_CONSTEXPR_TOKEN_
#define _WEBSOCKETPP_CPP11_STRICT_
#define ASIO_STANDALONE
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT

#include <websocketpp/config/asio_no_tls_client.hpp>

#include <websocketpp/client.hpp>

#include <iostream>

#include "../Core/Thread.h"

#include "../DebugNew.h"

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


namespace Atomic
{

struct WebSocketInternalState : public Thread
{
  /// URL.
  String url;
  /// Error string. Empty if no error.
  String error;
  /// Connection state.
  WebSocketState state;
  /// Mutex for synchronizing access to messages.
  mutable Mutex mutex;
  /// Unread messages.
  List<String> messages;
  /// WebSocket client.
  client c;
  /// WebSocket connection.
  client::connection_ptr con;

  void OnOpen(websocketpp::connection_hdl hdl)
  {
    state = WS_OPEN;
    LOGDEBUG("WebSocket CONNECTED to: " + url);
  }

  void OnClose(websocketpp::connection_hdl hdl)
  {
    state = WS_CLOSED;
    LOGDEBUG("WebSocket DISCONNECTED from: " + url);
  }

  void OnFail(websocketpp::connection_hdl hdl)
  {
    state = WS_FAIL_TO_CONNECT;
    LOGDEBUG("WebSocket FAILED to connect to: " + url);
  }

  void OnMessage(websocketpp::connection_hdl hdl, message_ptr msg)
  {
    // TODO: Find out if anybody ever uses any opcode other than these 2.
    // TODO: Make a binary message queue that is simmilar to the text message queue.
    switch (msg->get_opcode())
    {
      case websocketpp::frame::opcode::text:
      {
        const std::string &payload(msg->get_payload());
        MutexLock lock(mutex);
        messages.Push(payload.c_str());
      } break;

      case websocketpp::frame::opcode::binary:
      {
        LOGWARNING("WebSocket binary message type is not yet supported");
      } break;

      default:
      {
        LOGWARNING("Unsupported WebSocket message type: " + String(int(msg->get_opcode())));
      } break;
    }
  }

  void ThreadFunction()
  {
    c.run(); // This runs handlers when they need to be run.
  }

  void MakeConnection()
  {
    websocketpp::lib::error_code ec;
    con = c.get_connection(url.CString(), ec);
    if (ec)
    {
      state = WS_ERROR;
      error = ec.message().c_str();
      LOGDEBUG("WebSocket error: " + error);
      return;
    }
    c.connect(con);
  }
};

WebSocket::WebSocket(const String& url) :
  is_(new WebSocketInternalState())
{
  is_->url = url.Trimmed();
  is_->state = WS_INITIALIZING;

  is_->c.clear_access_channels(websocketpp::log::alevel::all);
  is_->c.clear_error_channels(websocketpp::log::elevel::all);

  is_->c.init_asio();
  is_->c.set_open_handler(bind(&WebSocketInternalState::OnOpen, is_, ::_1));
  is_->c.set_close_handler(bind(&WebSocketInternalState::OnClose, is_, ::_1));
  is_->c.set_fail_handler(bind(&WebSocketInternalState::OnFail, is_, ::_1));
  is_->c.set_message_handler(bind(&WebSocketInternalState::OnMessage, is_, ::_1, ::_2));

  // so that c.run() won't return (in another thread) until c.stop() is called.
  is_->c.start_perpetual();

  LOGDEBUG("WebSocket request to URL " + is_->url);

  is_->Run();

  is_->MakeConnection();
}

WebSocket::~WebSocket()
{
  is_->c.stop(); // Make c.run() return.
  is_->Stop();   // Wait until the thread finishes.
  delete is_;
}

const String& WebSocket::GetURL() const
{
  return is_->url;
}

String WebSocket::GetError() const
{
  return is_->error;
}

WebSocketState WebSocket::GetState() const
{
  return is_->state;
}

unsigned WebSocket::GetNumberOfMessages() const
{
  MutexLock lock(is_->mutex);
  return is_->messages.Size();
}

String WebSocket::GetNextMessage()
{
  MutexLock lock(is_->mutex);
  if (is_->messages.Size() == 0)
  {
    return "";
  }
  String message(is_->messages.Front());
  is_->messages.PopFront();
  return message;
}

void WebSocket::Send(String message)
{
  is_->c.send(is_->con, message.CString(), message.Length(), websocketpp::frame::opcode::text);
}

void  WebSocket::Close()
{
  is_->state = WS_CLOSING;
  websocketpp::lib::error_code ec;
  LOGDEBUG("WebSocket atempting to close URL " + is_->url);
  is_->con->terminate(ec);
}

void WebSocket::OpenAgain()
{
  is_->state = WS_INITIALIZING;
  LOGDEBUG("WebSocket request (again) to URL " + is_->url);
  is_->MakeConnection();
}

}

#endif
