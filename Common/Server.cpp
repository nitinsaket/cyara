#include <iostream>
#include "Server.h"

/* Initialises Winsock, creates a listener socket and binds it to
 * the local address to listen for incoming connections
 */
bool Server::Init(const char* l_pPort       /* Port on which to bind */ ) {
    // To create listener socket for server
    // need to set server's address in struct addrinfo
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    // Initialise internet address family and protocol
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    int iResult = getaddrinfo(NULL, l_pPort, &hints, &result);
    if (iResult != 0) {
        LOG("getaddrinfo failed with error: " << iResult);

        // getaddrinfo failed
        return FAILURE;
    }

    // Create server listener socket
    m_ListenerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_ListenerSocket == INVALID_SOCKET) {
        LOG("socket failed with error: " << WSAGetLastError());

        // Cleanup on socket creation failure
        freeaddrinfo(result);
        return FAILURE;
    }

    // Bind listener socket to local address
    iResult = bind(m_ListenerSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        LOG("bind failed with error: " << WSAGetLastError());

        // Cleanup on bind failure
        freeaddrinfo(result);
        Close();
        return false;
    }

    // Free allocated memory
    freeaddrinfo(result);

    // Success
    return true;
}   // Init
// *********************************************************************************

/* Server socket listens for incominig connections from clients
 */
bool Server::Listen() {
    // Listen for as many connections as the system allows
    int iResult;
    iResult = listen(m_ListenerSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        LOG("listen failed with error: " << WSAGetLastError());

        // Cleanup on listen failure
        Close();
        return false;
    }

    // New connection detected
    return true;
}   // Listen
// *********************************************************************************

/* Accept new connection from client
 * Return the client socket for usage
 */
SOCKET Server::Accept() {
    // Accept a client socket
    SOCKET clientSocket = accept(m_ListenerSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        LOG("accept failed with error :" << WSAGetLastError());

        // Cleanup on accept failure
        Close();

        // Return invalid socket
        return INVALID_SOCKET;
    }

    // Return the client socket
    return clientSocket;
}   // Accept
// *********************************************************************************

/* Send data on network
 */
bool Server::Send(SOCKET l_cSocket,         /* Client socket */
                  const char* l_sData,      /* Raw data to send */
                  int l_nLen                /* Length of data */) {
    // Check if pointer to data is NULL or data length is zero
    if (NULL == l_sData || 0 == l_nLen) {
        // Nothing to send
        return true;
    }

    // Send data on the client socket
    int bytesSent;
    bytesSent = send(l_cSocket, l_sData, l_nLen, 0);
    if (bytesSent == SOCKET_ERROR) {
        LOG("send failed with error: " << WSAGetLastError());

        // Cleanup client socket on send error
        closesocket(l_cSocket);

        // Failed to send data
        return FAILURE;
    }

    // No more data will be sent on this socket so
    // shutting down this connection
    int result = shutdown(l_cSocket, SD_SEND);
    if (SOCKET_ERROR == result) {
        LOG("shutdown failed with error: " << WSAGetLastError());

        // Cleanup client socket on shutdown error
        closesocket(l_cSocket);

        // Shutdown failed
        return FAILURE;
    }

    // Success
    return true;
}   // Send
// *********************************************************************************

/* Receive data from network
 */
int Server::Receive(SOCKET l_cSocket,       /* Client socket */
                    char* l_sData,          /* Buffer to store received data */
                    int l_nLen              /* Buffer size */) {
    // Check if buffer pointer is NULL or it's length zero
    if (NULL == l_sData || 0 == l_nLen || l_nLen < GUID_SIZE) {
        // Nothing to read
        return -1;
    }

    // Receive data from network
    int bytesReceived(0);
    int totalBytesReceived(0);
    char recvBuffer[DEFAULT_BUFLEN];
    int recvBuffLen = DEFAULT_BUFLEN;

    // Keep reading the socket till the other side closes connection
    do {
        // Receive bytes from network
        bytesReceived = recv(l_cSocket, recvBuffer, recvBuffLen, 0);
        if (bytesReceived > 0) {
            // Store received data
            errno_t res = memcpy_s((void*)(&l_sData[totalBytesReceived]), l_nLen, (void*)recvBuffer, bytesReceived);
            if (0 != res) {
                LOG("memcpy_s failed with error : " << res);

                // Cleanup client's socket on error
                closesocket(l_cSocket);

                // memcpy_s failed
                return -1;
            }

            // Update total bytes received
            totalBytesReceived += bytesReceived;
        }
        else if (0 == bytesReceived) {
            // Client has closed the connection
        }
        else {
            LOG("recv failed with error : " << WSAGetLastError());

            // Cleanup client's socket on error
            closesocket(l_cSocket);

            // recv failed
            return -1;
        }
    } while (bytesReceived > 0);

    // Success
    return totalBytesReceived;
}   // Receive
// *********************************************************************************

/* Close Listener socket and reset
 */
void Server::Close() {
    // Listener
    if (INVALID_SOCKET != m_ListenerSocket) {
        // Close socket
        closesocket(m_ListenerSocket);
        m_ListenerSocket = INVALID_SOCKET;
    }
}   // Close
// *********************************************************************************