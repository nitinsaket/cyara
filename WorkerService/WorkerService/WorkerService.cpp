#include "WorkerService.h"

/* Worker Service constructor
 */
WorkerService::WorkerService(const char* l_pDataProviderAddr,     /* Data Provider's IP Address */
                             const char* l_pAggregatorAddr        /* Aggregator's IP Adddress */ ) {
    // Reset the addresses
    memset(m_DataProviderAddr, 0, IP_ADDR_LEN);
    memset(m_AggregatorAddr, 0, IP_ADDR_LEN);

    // Copy the address params
    strncpy_s(m_DataProviderAddr, IP_ADDR_LEN, 
              l_pDataProviderAddr, strnlen_s(l_pDataProviderAddr, INET6_ADDRSTRLEN));
    strncpy_s(m_AggregatorAddr, IP_ADDR_LEN,
              l_pAggregatorAddr, strnlen_s(l_pAggregatorAddr, INET6_ADDRSTRLEN));

    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        LOG("WSAStartup failed with error: " << iResult);

        // Error initialising Winsock, exit process
        throw std::runtime_error("WSAStartup failed.");
    }
}   // WorkerService
// *********************************************************************************

/* Worker Service destructor
 */
WorkerService::~WorkerService() {
    // Cleanup
    WSACleanup();
}   // ~WorkerService
// *********************************************************************************

/* Connect to Data Provider Service, receive GUID,
 * connect to Aggregator Service, send digit count from GUID
 */
void WorkerService::Process() {
    // Connect to Data Provider
    if (!m_DataProviderClient.Connect(m_DataProviderAddr, DATAPROVIDER_PORT)) {
        // Connection attempt to server failed. Exit proceess
        throw std::runtime_error("Connect to Data Provider Service failed.");
    }

    // Initialise buffer to receive GUID
    char buffer[DEFAULT_BUFLEN];
    memset(buffer, 0, DEFAULT_BUFLEN);
    int bytesReceived(0);

    // Receive GUID from Data Provider
    bytesReceived = m_DataProviderClient.Receive(buffer, DEFAULT_BUFLEN);
    if (-1 == bytesReceived || bytesReceived != GUID_SIZE) {
        LOG("ERROR: Received (" << bytesReceived
            << ") bytes for GUID instead of (" << GUID_SIZE << ") bytes. Exiting.");

        // Invalid data received from network. Exit proceess
        throw std::runtime_error("Invalid data received from Data Provider.");
    }

    // Get count of digits([0-9]) in the GUID
    int count(__CountDigitsInGUID(buffer));
    if (-1 == count) {
        // Count digits in GUID failed. Exit proceess
        throw std::runtime_error("Counting of digits in GUID failed.");
    }

    // Connect to Aggreagator Service
    if (!m_AggregatorClient.Connect(m_AggregatorAddr, AGGREGATOR_PORT)) {
        // Connect to Aggregator Service failed. Exit proceess
        throw std::runtime_error("Connect to Aggregator Service failed.");
    }

    // Send count to Aggregator Service
    if (!m_AggregatorClient.Send((char*)&count, sizeof(count))) {
        // Sending count to Aggregator Service failed. Exit proceess
        throw std::runtime_error("Sending count to Aggregator Service failed.");
    }
}   // Process
// *********************************************************************************

/* Count the digits in a GUID
 * Ex: In GUID "D6EC0734-A304-4F61-8856-AAC010A680DD"
 *     number of digits ([0 - 9]) is: 21
 */
int WorkerService::__CountDigitsInGUID(const char* l_pGUIDBuff /* Buffer with received GUID */) {
    // Get guid from buffer
    GUID guid;
    errno_t res = memcpy_s(&guid, GUID_SIZE, l_pGUIDBuff, GUID_SIZE);
    if (0 != res) {
        LOG("Failed to copy GUID. Error : " << res);
        return -1;
    }

    // Extract GUID into a string
    wchar_t sGUID[100];
    memset(sGUID, 0, 100 * sizeof(wchar_t));
    if (!StringFromGUID2(guid, sGUID, 100)) {
        LOG("StringFromGUID2 failed");
        return -1;
    }

    // Count digits within the GUID
    int count(0);
    for (size_t i = 0; i < GUIDSTRLEN; ++i) {
        if (sGUID[i] >= L'0' && sGUID[i] <= L'9') {
            ++count;
        }
    }
    PRINTGUID(guid);
    DEBUGLOG("Digit Count: " << count);

    // Return number of digits in the GUID
    return count;
}   // __CountDigitsInGUID
// *********************************************************************************
