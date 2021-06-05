#include "Aggregator.h"
#include <thread>
#include <limits>

/* Thread function to generate report
 */
void ThreadReportGenerator(void * l_pData) {
    // Get pointer to the secure count list
    CountList* pCountList = static_cast<CountList*>(l_pData);

    // Infinite loop to generate report whenever user wants
    while (true) {
        // Clear screen
        std::cout << std::string(10, '\n') << std::endl;

        // Get input from user
        LOG("AGGREGATOR");
        LOG("==========");
        std::cout << "Press '1' to generate report: ";

        // Read user input
        int input(-1);
        if (!(std::cin >> input)) {
            // Invalid key press
            std::cin.clear();
            std::cin.ignore(LLONG_MAX, '\n');
            continue;
        }

        // Invalid key press
        if (input != 1) {
            continue;
        }

        // Process the data in the list
        int minCount(0);
        int maxCount(0);
        // double avgCount(0.0);
        int totalDigits(0);
        int totalResponses(0);

        // Want to exclusively access Count list
        pCountList->m_Mutex.lock();

        // Iterate through the list to generate report
        for (std::list<int>::const_iterator itr(pCountList->m_CountList.begin());
                                            itr != pCountList->m_CountList.end();
                                            ++itr) {
            // Get count
            int count = *itr;

            // Initialise on first iteration
            if (0 == totalResponses) {
                minCount = count;
                maxCount = count;
            }

            // Set min digit
            if (count < minCount) {
                minCount = count;
            }

            // Set max digit
            if (count > maxCount) {
                maxCount = count;
            }

            // Total Digit Count
            totalDigits += count;

            // Total number of responses from Worker Service
            ++totalResponses;
        }

        // Relinquish lock
        pCountList->m_Mutex.unlock();

        // Display Report
        LOG("==========================");
        LOG("Min:               " << minCount);
        LOG("Max:               " << maxCount);
        LOG("Average:           " << (totalResponses == 0? 0 : (totalDigits*1.0/totalResponses)));
        LOG("Total Digits:      " << totalDigits);
        LOG("Total Responses:   " << totalResponses);
        LOG("==========================");
        LOG("\n\nPress any key to continue...");
        std::cin.get();
        std::cin.get();
    }
}   // ThreadReportGenerator
// *********************************************************************************

/* Constructor
 */
Aggregator::Aggregator() {
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        LOG("WSAStartup failed with error: " << iResult);

        // Error initialising Winsock, exit process
        throw std::runtime_error("WSAStartup failed.");
    }
}   // Aggregator
// *********************************************************************************

/* Destructor
 */
Aggregator::~Aggregator() {
    // Cleanup
    WSACleanup();
}   // ~Aggregator
// *********************************************************************************

/* Connect to client, receive count, store count in list
 */
void Aggregator::Process() {
    // Start a thread that will generate report
    std::thread reportGenerator(ThreadReportGenerator, (void*)&m_CountStruct);

    // Initialise the server
    if (!m_Server.Init(AGGREGATOR_PORT)) {
        // Could not initialise server. Exit process
        throw std::runtime_error("Server initialisation failed. Exiting.");
    }

    // Aggregator expects unlimited connections from clients
    // so the below procedure will be carried out for ever
    while (true) {
        // Listen for incoming connection
        if (!m_Server.Listen()) {
            // Listen for incoming connection fails. Exit process
            throw std::runtime_error("Listen for incoming connection fails. Exiting.");
        }

        // Accept the connection received from a client
        SOCKET client = m_Server.Accept();
        if (INVALID_SOCKET == client) {
            // Accept fails for incoming connection. Exit process
            throw std::runtime_error("Accept fails for incoming connection. Exiting.");
        }

        // Receive digit count from client
        char buff[20];
        memset(buff, 0, 20);
        if (-1 == m_Server.Receive(client, buff, 20)) {
            // Failed to Receive data. close client socket and continue the process
            closesocket(client);
            continue;
        }

        // Exclusively access Count list
        m_CountStruct.m_Mutex.lock();
        //m_Mutex.lock();

        // Update count list
        m_CountStruct.m_CountList.push_back(*((int*)buff));

        // Relinquish access
        m_CountStruct.m_Mutex.unlock();

        // Close client socket
        closesocket(client);
    }

    // This code will never hit as current thread and reportGenerator thread
    // are in an infinite loop. There are ways to inform reportGenerator thread
    // to exit but that job will be taken up if I have time left.
    // reportGenerator.join();
}   // Process
// *********************************************************************************
