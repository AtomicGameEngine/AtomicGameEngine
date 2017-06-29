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

#include <easy/easy_socket.h>

#include <string.h>
#include <thread>

#ifdef _WIN32
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#else
#include <errno.h>
#include <sys/ioctl.h>
#endif

bool EasySocket::checkSocket(socket_t s) const
{
    return s > 0;
}

int EasySocket::_close(EasySocket::socket_t s)
{
#ifdef _WIN32
    return ::closesocket(s);
#else
    return ::close(s);
#endif
}

void EasySocket::setBlocking(EasySocket::socket_t s, bool blocking)
{

#ifdef _WIN32
    u_long iMode = blocking ? 0 : 1;//0 - blocking, 1 - non blocking
    ioctlsocket(s, FIONBIO, &iMode);
#else
    const int iMode = blocking ? 0 : 1;//0 - blocking, 1 - non blocking
    ioctl(s, FIONBIO, (char *)&iMode);
#endif
}

int EasySocket::bind(uint16_t portno)
{
    if (!checkSocket(m_socket)) return -1;

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    auto res = ::bind(m_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    return res;
}

void EasySocket::flush()
{
    if (m_socket){
        _close(m_socket);
    }
    if (m_replySocket != m_socket){
        _close(m_replySocket);
    }
#ifdef _WIN32
    m_socket = 0;
    m_replySocket = 0;
#else
    wsaret = 0;
#endif
}

void EasySocket::checkResult(int result)
{
    //printf("Errno: %s\n", strerror(errno));
    if(result >= 0){
        m_state = CONNECTION_STATE_SUCCESS;
        return;
    }else if(result == -1){


        int error_code = 0;

#ifdef _WIN32
        error_code = WSAGetLastError();
        const int CONNECTION_ABORTED = WSAECONNABORTED;
        const int CONNECTION_RESET = WSAECONNRESET;
        const int CONNECTION_IN_PROGRESS = WSAEINPROGRESS;
#else
        error_code = errno;
        const int CONNECTION_ABORTED = ECONNABORTED;
        const int CONNECTION_RESET = ECONNRESET;
        const int CONNECTION_IN_PROGRESS = EINPROGRESS;
        const int CONNECTION_BROKEN_PIPE = EPIPE;
        const int CONNECTION_ENOENT = ENOENT;
#endif

        switch(error_code)
        {
        case CONNECTION_ABORTED:
        case CONNECTION_RESET:
#ifndef _WIN32
        case CONNECTION_BROKEN_PIPE:
        case CONNECTION_ENOENT:
#endif
            m_state = CONNECTION_STATE_DISCONNECTED;
            break;
        case CONNECTION_IN_PROGRESS:
            m_state = CONNECTION_STATE_IN_PROGRESS;
            break;
        default:
            break;
        }
    }
}

void EasySocket::init()
{
    if (wsaret == 0)
    {
        int protocol = 0;
#ifdef _WIN32
        protocol = IPPROTO_TCP;
#endif
        m_socket = socket(AF_INET, SOCK_STREAM, protocol);
        if (!checkSocket(m_socket)) {
            return;
        }
    }else
        return;

    setBlocking(m_socket,true);
#ifndef _WIN32
        wsaret = 1;
#endif
    int opt = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
}

EasySocket::EasySocket()
{
#ifdef _WIN32
    WSADATA wsaData;
    wsaret = WSAStartup(0x101, &wsaData);
#else
    wsaret = 0;
#endif
    init();
#ifndef _WIN32
    wsaret = 1;
#endif
}

EasySocket::~EasySocket()
{
    flush();
#ifdef _WIN32
    if (wsaret == 0)
        WSACleanup();
#endif
}

int EasySocket::send(const void *buf, size_t nbyte)
{
    if(!checkSocket(m_replySocket))  return -1;
    int res = 0;
#if defined(_WIN32) || defined(__APPLE__)
    res = ::send(m_replySocket, (const char*)buf, (int)nbyte, 0);
#else
    res = ::send(m_replySocket,buf,nbyte,MSG_NOSIGNAL);
#endif
    checkResult(res);
    return res;
}

int EasySocket::receive(void *buf, size_t nbyte)
{
    if(!checkSocket(m_replySocket))  return -1;
    int res = 0;
#ifdef _WIN32
    res = ::recv(m_replySocket, (char*)buf, (int)nbyte, 0);
#else
    res = ::read(m_replySocket,buf,nbyte);
#endif

    checkResult(res);
    if (res == 0){
        m_state = CONNECTION_STATE_DISCONNECTED;
    }
    return res;
}

int EasySocket::listen(int count)
{
    if(!checkSocket(m_socket)) return -1;
    int res = ::listen(m_socket,count);
    checkResult(res);
    return res;
}

int EasySocket::accept()
{
    if(!checkSocket(m_socket)) return -1;

    fd_set fdread, fdwrite, fdexcl;
    timeval tv = { 0 };
    FD_ZERO (&fdread);
    FD_SET (m_socket, &fdread);
    fdwrite = fdread;
    fdexcl = fdread;
    tv.tv_sec = 0; tv.tv_usec = 500;

    int rc =select (m_socket+1, &fdread, &fdwrite, &fdexcl, &tv);

    if(rc <= 0){
        //there is no connection for accept
        return -1;
    }
    m_replySocket = ::accept(m_socket,nullptr,nullptr);

    checkResult((int)m_replySocket);
    if(checkSocket(m_replySocket))
    {
        int send_buffer = 64*1024*1024;
        int send_buffer_sizeof = sizeof(int);
        setsockopt(m_replySocket, SOL_SOCKET, SO_SNDBUF, (char*)&send_buffer, send_buffer_sizeof);
        
        //int flag = 1;
        //int result = setsockopt(m_replySocket,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(int));

        // Apple doesn't have MSG_NOSIGNAL, work around it
#ifdef __APPLE__
        int value = 1;
        setsockopt(m_replySocket, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value));
#endif

        //setBlocking(m_replySocket,true);
    }
    return (int)m_replySocket;
}

bool EasySocket::setAddress(const char *serv, uint16_t portno)
{
    server = gethostbyname(serv);
    if (server == NULL) {
        return false;
        //fprintf(stderr,"ERROR, no such host\n");
    }
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    return true;
}

int EasySocket::connect()
{
    if (server == NULL || m_socket <=0 ) {
        return -1;
        //fprintf(stderr,"ERROR, no such host\n");
    }
    int res = 0;
    //TODO: more intelligence
#ifndef _WIN32
    setBlocking(m_socket,false);

    int counter = 0;
    int sleepMs = 20;
    int waitSec = 1;
    int waitMs = waitSec*1000/sleepMs;
    
    while(counter++ < waitMs)
    {
        res = ::connect(m_socket,(struct sockaddr *) &serv_addr,sizeof(serv_addr));

        // on Apple, treat EISCONN error as success
#ifdef __APPLE__
        if (res == -1 && errno == EISCONN)
        {
            res = 0;
            break;
        }
#endif

        checkResult(res);

        if (res == 0)
            break;

        if (m_state == CONNECTION_STATE_IN_PROGRESS)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
            continue;
        }


        if(m_state != CONNECTION_STATE_IN_PROGRESS && m_state != CONNECTION_STATE_SUCCESS )
            break;
    }

    setBlocking(m_socket,true);
#else
    res = ::connect(m_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    checkResult(res);
#endif
    if(res == 0){

        struct timeval tv;

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

#ifdef __APPLE__
        // Apple doesn't have MSG_NOSIGNAL, work around it
        int value = 1;
        setsockopt(m_socket, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value));
#endif

        m_replySocket = m_socket;
    }
    return res;
}
