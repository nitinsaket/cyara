#include <iostream>
#include "DataProvider.h"

/* Data Provider constructor
 */
DataProvider::DataProvider() {
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error: " << iResult << std::endl;

        // Error initialising Winsock, exit process
        throw std::runtime_error("WSAStartup failed.");
    }
}   // DataProvider
// *********************************************************************************

/* Data Provider destructor
 */
DataProvider::~DataProvider() {
    // Cleanup
    WSACleanup();
}   // DataProvider
// *********************************************************************************

// The main functionality of Data Provider
void DataProvider::Process() {
    // Initialise the server
    if (!m_Server.Init(DATAPROVIDER_PORT)) {
        LOG("Server initialisation failed. Exiting.");

        // Could not initialise server. Stop process
        return;
    }

    // Data Provider expects unlimited connections from clients
    // so the below procedure will be carried out for ever
    while (true) {
        // Listen for incoming connection
        if (!m_Server.Listen()) {
            // Listen failed, exit process
            throw std::runtime_error("Listen for incoming connection failed. Exiting.");
        }

        // Accept the connection received from a client
        SOCKET client = m_Server.Accept();
        if (INVALID_SOCKET == client) {
            // Accept failed, exit process
            throw std::runtime_error("Accept returns invalid socket. Exiting.");
        }

        // Generate a GUID for client
        GUID guid;
        if (!__GenerateGUID(guid)) {
            // If GUID generation failed then close client socket and continue
            closesocket(client);
            continue;
        }

        // Send GUID to client
        if (!__SendGUID(client, guid)) {
            // If send GUID to client failed then close client socket and continue
            closesocket(client);
            continue;
        }

        // This is needed for proof
        PRINTGUID(guid);

        // Close the client socket
        closesocket(client);
    }
}   // Process
// *********************************************************************************

/* Function to generate a GUID
 */
bool DataProvider::__GenerateGUID(GUID& l_GUID  /* GUID struct to update */) {
    // Generate the GUID
    HRESULT res = CoCreateGuid(&l_GUID);
    if (S_OK != res) {
        LOG("CoCreateGuid returned status " << res);

        // GUID generation failed
        return false;
    }

    // Success
    return true;
}   // __GenerateGUID
// *********************************************************************************

/* Send GUID to the client
 */
bool DataProvider::__SendGUID(SOCKET l_Client,      /* Client socket */
                              const GUID& l_GUID    /* GUID to be sent */ ) {
    // Buffer to store GUID into
    char buffer[DEFAULT_BUFLEN];
    memset(buffer, 0, DEFAULT_BUFLEN);
    memcpy_s(buffer, DEFAULT_BUFLEN, &l_GUID, GUID_SIZE);

    // Send the GUID to client
    return m_Server.Send(l_Client, buffer, GUID_SIZE);
}   // __SendGUID
// *********************************************************************************
