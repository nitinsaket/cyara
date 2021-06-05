#if !defined(__SERVER_H__)
#define __SERVER_H__

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include "Commons.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

// Server
class Server {
public:
    /* Constructor
     */
    Server() {
        // Initialise listener socket
        m_ListenerSocket = INVALID_SOCKET;
    }

    /* Destructor
     */
    ~Server() {
        // Close all sockets
        Close();
    }

    // Initialize Winsock and server's listener socket
    bool Init(const char* l_pPort);

    // Listen for incoming connections
    bool Listen();

    // Accept new connection
    SOCKET Accept();

    // Send data on network
    bool Send(SOCKET l_cSocket, const char* l_sData, int l_nLen);

    // Receive data from the network
    int Receive(SOCKET l_cSocket, char* l_sData, int l_nLen);

    // Close socket
    void Close();

private:
    // Server's listener socket
    SOCKET m_ListenerSocket;

};  // Server

#endif