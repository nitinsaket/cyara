#include <iostream>
#include "Client.h"

/* Connect to the server using IP address and port.
 */
bool Client::Connect(const char* l_sIPAddress,        /* IP Address of the server */
                     const char* l_sPort              /* Port at which to connect */ ) {
    // Get Server's IP and port 
    struct addrinfo *addrList = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;
    int iResult;

    // Initialise internet address family and protocol
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve server's address and port
    iResult = getaddrinfo(l_sIPAddress, l_sPort, &hints, &addrList);
    if (iResult != 0) {
        LOG("getaddrinfo failed with error: " << iResult);

        // Connection to server failed
        return FAILURE;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = addrList; ptr != NULL; ptr = ptr->ai_next) {
        // Create a SOCKET for connecting to server
        m_Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (INVALID_SOCKET == m_Socket) {
            LOG("socket failed with error: " << WSAGetLastError());

            // Connection to server failed
            return FAILURE;
        }

        // Connect to server
        iResult = connect(m_Socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            Close();
            continue;
        }

        // Connected
        break;
    }

    // Free address list
    freeaddrinfo(addrList);

    // Could not connect to server
    if (m_Socket == INVALID_SOCKET) {
        LOG("Unable to connect to server!");
        return FAILURE;
    }

    // Connection successful
    return SUCCESS;
}   // Connect
// *********************************************************************************

/* Send data over the network
 */
bool Client::Send(const char* l_sData,      /* Data to send over network */
                  int l_nLen                /* Length of data */ ) {
    // Check if pointer to data is NULL or data length is zero
    if (NULL == l_sData || l_nLen <= 0) {
        // Nothing to send
        return true;
    }

    // Send data on the client socket
    int bytesSent;
    bytesSent = send(m_Socket, l_sData, l_nLen, 0);
    if (bytesSent == SOCKET_ERROR) {
        LOG("send failed with error: " << WSAGetLastError());

        // Cleanup on send error
        Close();
        return false;
    }

    // No more data will be sent on this socket so
    // shutting down this connection
    int result = shutdown(m_Socket, SD_SEND);
    if (SOCKET_ERROR == result) {
        LOG("shutdown failed with error: " << WSAGetLastError());

        // Cleanup on shutdown error
        Close();
        return false;
    }

    // Success
    return true;
}   // Send
// *********************************************************************************

/** Receive data from the network
 * Returns total bytes received
 */
int Client::Receive(char* l_sData,      /* Buffer to receive data */
                    int l_nLen          /* Buffer size */ ) {
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
        bytesReceived = recv(m_Socket, recvBuffer, recvBuffLen, 0);
        if (bytesReceived > 0) {
            // Store received data
            errno_t res = memcpy_s((void*)(&l_sData[totalBytesReceived]), l_nLen, (void*)recvBuffer, bytesReceived);
            if (0 != res) {
                LOG("memcpy_s failed with error : " << res);

                // Close socket
                Close();
                return -1;
            }

            // Update total bytes received
            totalBytesReceived += bytesReceived;
        }
        else if (0 == bytesReceived) {
            // Server closed connection
        }
        else {
            LOG("recv failed with error : " << WSAGetLastError());

            // Close socket
            Close();
            return -1;
        }
    } while (bytesReceived > 0);

    // Success
    return totalBytesReceived;
}   // Receive
// *********************************************************************************

/* Close socket connected to server
 */
void Client::Close() {
    if (INVALID_SOCKET != m_Socket) {
        // Close socket and reset
        closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
    }
}   // Close
// *********************************************************************************