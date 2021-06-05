#if !defined(__CLIENT_H__)
#define __CLIENT_H__

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Commons.h"


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

// Client
class Client {
public:
    // Constructor
    Client() {
        // Initialise socket
        m_Socket = INVALID_SOCKET;
    }

    // Destructor
    ~Client() {
        // Close socket
        Close();
    }

    // Connect to server
    bool Connect(const char* l_sIPAddress, const char* l_sPort);

    // Send data to network
    bool Send(const char* l_sData, int l_nLen);

    // Receive data from the network
    int Receive(char* l_sData, int l_nLen);

    // Close socket
    void Close();

private:

    // Socket to connect to server
    SOCKET m_Socket;

};  // Client

#endif