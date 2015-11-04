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
#include "../Web/WebSocket.h"

#ifdef EMSCRIPTEN

#include "../DebugNew.h"

// Add code to use an HTML5 style WebSocket here.

#else

#include "../Web/WebInternalConfig.h"
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

struct WebSocketInternalState
{
  /// The work queue.
  asio::io_service service;
  /// The WebSocket external state.
  WebSocket &es;
  /// URL.
  String url;
  /// Error string. Empty if no error.
  String error;
  /// Connection state.
  WebSocketState state;
  /// WebSocket client.
  client c;
  /// WebSocket connection.
  client::connection_ptr con;

  WebSocketInternalState(WebSocket &es_)
    : es(es_)
  {
  }

  ~WebSocketInternalState()
  {
  }

  void OnOpen(websocketpp::connection_hdl hdl)
  {
    state = WS_OPEN;
    LOGDEBUG("WebSocket CONNECTED to: " + url);
    es.SendEvent("open");
  }

  void OnClose(websocketpp::connection_hdl hdl)
  {
    state = WS_CLOSED;
    LOGDEBUG("WebSocket DISCONNECTED from: " + url);
    es.SendEvent("close");
  }

  void OnFail(websocketpp::connection_hdl hdl)
  {
    state = WS_FAIL_TO_CONNECT;
    LOGDEBUG("WebSocket FAILED to connect to: " + url);
    es.SendEvent("fail_to_connect");
  }

  void OnMessage(websocketpp::connection_hdl hdl, message_ptr msg)
  {
    VariantMap eventData;
    const std::string &payload(msg->get_payload());

    eventData.Insert(MakePair(StringHash("type"), Variant(int(msg->get_opcode()))));

    switch (msg->get_opcode())
    {
      case websocketpp::frame::opcode::text:
        eventData.Insert(MakePair(StringHash("data"), Variant(String(payload.data(), payload.length()))));
        es.SendEvent("message", eventData);
        break;

      case websocketpp::frame::opcode::binary:
        eventData.Insert(MakePair(StringHash("data"), Variant(PODVector<unsigned char>((const unsigned char *)payload.data(), payload.length()))));
        es.SendEvent("message", eventData);
        break;

      default:
        eventData.Insert(MakePair(StringHash("data"), Variant(String(payload.data(), payload.length()))));
        LOGWARNING("Unsupported WebSocket message type: " + String(int(msg->get_opcode())));
        break;
    }
  }

  void MakeConnection()
  {
    websocketpp::lib::error_code ec;
    con = c.get_connection(url.CString(), ec);
    if (ec)
    {
      state = WS_INVALID;
      error = ec.message().c_str();
      LOGDEBUG("WebSocket error: " + error);
      es.SendEvent("invalid");
      return;
    }
    c.connect(con);
  }
};

WebSocket::WebSocket(Context* context, const String& url) :
  Object(context),
  is_(new WebSocketInternalState(*this))
{
  is_->url = url.Trimmed();
  is_->state = WS_CONNECTING;

  is_->c.clear_access_channels(websocketpp::log::alevel::all);
  is_->c.clear_error_channels(websocketpp::log::elevel::all);
}

void WebSocket::setup(asio::io_service *service)
{
  websocketpp::lib::error_code ec;
  is_->c.init_asio(service, ec);
  if (ec)
  {
    is_->state = WS_INVALID;
    is_->error = ec.message().c_str();
    LOGDEBUG("WebSocket error: " + is_->error);
    SendEvent("invalid");
    return;
  }
  is_->c.set_open_handler(bind(&WebSocketInternalState::OnOpen, is_, ::_1));
  is_->c.set_close_handler(bind(&WebSocketInternalState::OnClose, is_, ::_1));
  is_->c.set_fail_handler(bind(&WebSocketInternalState::OnFail, is_, ::_1));
  is_->c.set_message_handler(bind(&WebSocketInternalState::OnMessage, is_, ::_1, ::_2));

  LOGDEBUG("WebSocket request to URL " + is_->url);

  is_->MakeConnection();
}

WebSocket::~WebSocket()
{
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
  is_->state = WS_CONNECTING;
  LOGDEBUG("WebSocket request (again) to URL " + is_->url);
  is_->MakeConnection();
}

}

#endif
