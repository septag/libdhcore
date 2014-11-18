/***********************************************************************************
 * Copyright (c) 2012, Sepehr Taghdisian
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 ***********************************************************************************/

#ifndef __NETSOCKET_H__
#define __NETSOCKET_H__

#include "types.h"
#include "core-api.h"

/**
 * @defgroup socket Sockets
 */

#if defined(_WIN_)
#include <WinSock2.h>
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

#if defined(_WIN_)
  #define SOCK_NULL      INVALID_SOCKET
  #define SOCK_ERROR     SOCKET_ERROR
#else
  #define SOCK_NULL      -1
  #define SOCK_ERROR     -1
#endif

/* */
result_t sock_init();
void sock_release();

/**
 * Get current host name
 * @ingroup socket
 */
CORE_API const char* sock_gethostname();

/**
 * Resolves ip address of network name (dns resolve)
 * @ingroup socket
 */
CORE_API char* sock_resolveip(const char* name, OUT char *ipaddr);

/**
 * creates udp socket, udp sockets are connection-less but not very stable/reliable
 * @ingroup socket
 */
CORE_API socket_t sock_udp_create();

/**
 * destroy udp socket
 * @ingroup socket
 */
CORE_API void sock_udp_destroy(socket_t sock);

/**
 * binds port to udp socket in order to recv data from that port
 * receiver should always bind a port number to the socket before receiving data
 * @ingroup socket
 */
CORE_API result_t sock_udp_bind(socket_t sock, int port);

/**
 * receives data from udp socket
 * @param out_sender_ipaddr ip address of the sender returned
 * @return number of bytes actually received, <=0 if error occured
 * @ingroup socket
 */
CORE_API int sock_udp_recv(socket_t sock, void* buffer, int size, char* out_sender_ipaddr);

/**
 * sends data to udp socket
 * @param ipaddr ip address of the target receiver returned
 * @param port port address that target is listening
 * @param buffer buffer to be sent
 * @return number of bytes actually sent, <=0 if error occured
 * @ingroup socket
 */
CORE_API int sock_udp_send(socket_t sock, const char* ipaddr, int port, const void* buffer, 
    int size);

/**
 * create tcp socket, tcp sockets need connection (accept/connect) but are stable and reliable
 * @ingroup socket
 */
CORE_API socket_t sock_tcp_create();

/**
 * destroy tcp socket
 * @ingroup socket
 */
CORE_API void sock_tcp_destroy(socket_t sock);

/**
 * listens tcp socket as a server and waits (blocks) the program until peer is connected
 * @ingroup socket
 */
CORE_API result_t sock_tcp_listen(socket_t sock, int port);

/**
 * accept peer connection
 * @return newly connected/created socket, user should send/recv data with newly created socket
 * @ingroup socket
 */
CORE_API socket_t sock_tcp_accept(socket_t sock, char* out_peer_ipaddr);

/**
 * connect to server (listening) socket, blocks the program until it connects to peer
 * @param ipaddr ip address of peer to connect
 * @param port port number of the connection
 * @ingroup socket
 */
CORE_API result_t sock_tcp_connect(socket_t sock, const char* ipaddr, int port);

/**
 * receives data from tcp peer
 * @param buffer receive buffer, must hold maximum amount of 'size'
 * @param size maximum buffer size (bytes)
 * @return actual bytes that is received. <=0 if error occured
 * @ingroup socket
 */
CORE_API int sock_tcp_recv(socket_t sock, void* buffer, int size);

/**
 * sends data to tcp peer
 * @param buffer buffer to be sent
 * @param size size of the send buffer (bytes)
 * @return actual bytes that is sent. <=0 if error occured
 * @ingroup socket
 */
CORE_API int sock_tcp_send(socket_t sock, const void* buffer, int size);

/**
 * blocks the program and checks if socket has input packet for receiving buffer
 * @param timeout timeout in milliseconds
 * @ingroup socket
 */
CORE_API int sock_poll_recv(socket_t sock, uint timeout);

/**
 * blocks the program and checks if we can send data through the socket
 * @param timeout timeout in milliseconds
 * @ingroup socket
 */
CORE_API int sock_poll_send(socket_t sock, uint timeout);

#ifdef __cplusplus
namespace dh {

// TCP
class SocketTCP
{
private:
    socket_t m_sock = SOCK_NULL;

private:
    SocketTCP(socket_t sock) : m_sock(sock) {}

public:
    SocketTCP()
    {
        m_sock = sock_tcp_create();
    }

    ~SocketTCP()
    {
        if (m_sock != SOCK_NULL && m_sock != SOCK_ERROR)
            sock_tcp_destroy(m_sock);
        m_sock = SOCK_NULL;
    }

    bool listen(int port)
    {
        return sock_tcp_listen(m_sock, port) != RET_FAIL ? true : false;
    }

    SocketTCP accept(char *peer_addr = nullptr)
    {
        return SocketTCP(sock_tcp_accept(m_sock, peer_addr));
    }

    bool connect(const char *addr, int port)
    {
        return sock_tcp_connect(m_sock, addr, port) != RET_FAIL ? true : false;
    }

    bool poll_recv(uint timeout = UINT32_MAX)
    {
        return static_cast<bool>(sock_poll_recv(m_sock, timeout));
    }

    bool poll_send(uint timeout = UINT32_MAX)
    {
        return static_cast<bool>(sock_poll_send(m_sock, timeout));
    }

    int recv(void *buffer, int size)
    {
        return sock_tcp_recv(m_sock, buffer, size);
    }

    int send(const void *buffer, int size)
    {
        return sock_tcp_send(m_sock, buffer, size);
    }

    operator socket_t() {   return m_sock;  }
    bool is_open() const    {   return m_sock != SOCK_NULL; }
};

// UDP
class SocketUDP
{
private:
    socket_t m_sock = SOCK_NULL;

public:
    SocketUDP()
    {
        m_sock = sock_udp_create();
    }

    ~SocketUDP()
    {
        if (m_sock != SOCK_NULL && m_sock != SOCK_ERROR)
            sock_udp_destroy(m_sock);
        m_sock = SOCK_NULL;
    }

    bool bind(int port)
    {
        return sock_udp_bind(m_sock, port) != RET_FAIL ? true : false;
    }

    bool poll_recv(uint timeout = UINT32_MAX)
    {
        return static_cast<bool>(sock_poll_recv(m_sock, timeout));
    }

    bool poll_send(uint timeout = UINT32_MAX)
    {
        return static_cast<bool>(sock_poll_send(m_sock, timeout));
    }

    int recv(void *buffer, int size, char *peer_addr = nullptr)
    {
        return sock_udp_recv(m_sock, buffer, size, peer_addr);
    }

    int send(const void *buffer, int size, const char *addr, int port)
    {
        return sock_udp_send(m_sock, addr, port, buffer, size);
    }

    operator socket_t() {   return m_sock;  }
    bool is_open() const    {   return m_sock != SOCK_NULL; }
};

} // dh
#endif

#endif /* __NETSOCKET_H__ */
