#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include <iostream>
#include <string>
#include <thread>
#include "Client.h"
#include "Server.h"
#include "DataProvider.h"
#include "Aggregator.h"
#include "WorkerService.h"

void InitWinsock() {
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error: " << iResult << std::endl;

        // Error initialising Winsock, exit process
        throw std::runtime_error("WSAStartup failed.");
    }
}

void CleanupWinsock() {
    // Cleanup
    WSACleanup();
}

/* This Test case tests the Connect() function of Client class
 */
TEST_CASE("Client Connect Function") {
    // Initialise Winsock
    InitWinsock();

    // Create a client
    Client c;

    // Client tries to connect when server does not exits
    SECTION("Server does not exist") {
        CHECK(c.Connect("localhost", "9876") != true);
    }

    // Create server which listens on port "7777"
    Server s;
    s.Init(DATAPROVIDER_PORT);
    s.Listen();

    // Client tries a wrong IP address
    SECTION("Client connects to wrong IP address") {
        CHECK(c.Connect("154.210.20.10", DATAPROVIDER_PORT) != true);
    }

    // Client tries an invalid IP address
    SECTION("Client connects to wrong IP address") {
        CHECK(c.Connect("554.610.720.810", DATAPROVIDER_PORT) != true);
    }

    // Client tries an invalid IP address
    SECTION("Client connects to wrong IP address") {
        CHECK(c.Connect("127.0.0", DATAPROVIDER_PORT) != true);
    }

    // Client tries to connect to server on wrong port
    SECTION("Client tries wrong port") {
        CHECK(c.Connect("localhost", "9876") != true);
    }

    // Client connects to 127.0.0.1 but on a wrong port
    SECTION("Client connects to 127.0.0.1 but on wrong port") {
        CHECK(c.Connect("127.0.0.1", "5687") != true);
    }

    // Client tries to connect to server on the correct port
    SECTION("Client tries to connect to the correct port") {
        CHECK(c.Connect("localhost", DATAPROVIDER_PORT) == true);
        s.Accept();

        // Close client for next test case
        c.Close();
    }

    // Client connects to 127.0.0.1 on correct port
    SECTION("Client connects to 127.0.0.1 and on correct port") {
        CHECK(c.Connect("127.0.0.1", DATAPROVIDER_PORT) == true);
        s.Accept();

        // Close client for next test case
        c.Close();
    }

    // Client connects to 127.0.0.10 on correct port
    SECTION("Client connects to 127.0.0.10 and on correct port") {
        CHECK(c.Connect("127.0.0.1", DATAPROVIDER_PORT) == true);
        s.Accept();

        // Close client for next test case
        c.Close();
    }

    // Cleanup Test Case and test Close function
    CHECK_NOTHROW(c.Close());
    CHECK_NOTHROW(s.Close());


    // Clean Winsock
    CleanupWinsock();
}

/* This Test case tests the Send() function of Client class
 */
TEST_CASE("Client Send Function") {
    // Initialise Winsock
    InitWinsock();

    // Create a client
    Client c;

    // Client tries to send NULL data on network
    SECTION("Client send NULL data") {
        CHECK(c.Send(NULL, 10) == true);
    }

    // Data buffer to send data out to the network
    char data[] = "ABCDEFG";
    int dataLen = 7;

    // Client tries to send data with wrong length
    SECTION("Client sends data with wrong length") {
        CHECK(c.Send(data, 0) == true);
    }

    // Client tries to send data with negative length
    SECTION("Client sends data with negative length") {
        CHECK(c.Send(data, -1) == true);
    }

    // Client sends data without connecting to Server
    SECTION("Client sends data without connection") {
        CHECK(c.Send(data, dataLen) != true);
    }

    // Create server which listens on port "7777" and connect it to client
    Server s;
    s.Init(DATAPROVIDER_PORT);
    s.Listen();
    c.Connect("127.0.0.1", DATAPROVIDER_PORT);
    s.Accept();

    // Client sends data successfully to Server
    SECTION("Client sends data successfully to Server") {
        CHECK(c.Send(data, dataLen) == true);
    }

    // Client sends dodgy data to Server
    SECTION("Client sends data with huge length to Server") {
        CHECK(c.Send(data, DEFAULT_BUFLEN) == true);
    }

    // Client sends less dodgy data to Server
    SECTION("Client sends data with smaller length to Server") {
        CHECK(c.Send(data, 2) == true);
    }

    // Cleanup Test Case
    c.Close();
    s.Close();

    // Clean Winsock
    CleanupWinsock();
}

/* This Test case tests the Receive() function of Client class
 */
TEST_CASE("Client Receive Function") {
    // Initialise Winsock
    InitWinsock();

    // Create a client
    Client c;

    // Client tries to receive data in a NULL buffer
    SECTION("Client receives data in NULL buffer") {
        CHECK(c.Receive(NULL, 0) == -1);
    }

    // Data buffer to send data out to the network
    char data[DEFAULT_BUFLEN] = {0};

    // Buffer length 0
    SECTION("Client receives data in buffer of size 0") {
        CHECK(c.Receive(data, 0) == -1);
    }

    // Buffer size less than GUID size
    SECTION("Client receives data in buffer of size less than GUID size") {
        CHECK(c.Receive(data, GUID_SIZE-1) == -1);
    }

    // Receive without connection
    SECTION("Client receives data without connecting to server") {
        CHECK(c.Receive(data, DEFAULT_BUFLEN) == -1);
    }

    // Initialise server
    Server s;
    s.Init(DATAPROVIDER_PORT);
    s.Listen();
    c.Connect("localhost", DATAPROVIDER_PORT);
    SOCKET clientSocket = s.Accept();

    // Receive data
    SECTION("Client receives data after connecting to server") {
        s.Send(clientSocket, "ABCDEF", 6);
        CHECK(c.Receive(data, DEFAULT_BUFLEN) == 6);
        std::string str(data);
        CHECK(str.compare("ABCDEF") == 0);
    }

    // Clean Winsock
    CleanupWinsock();
}

/* This Test case tests the Init() function of Server class
 */
TEST_CASE("Server Init Function") {
    // Initialise Winsock
    InitWinsock();

    // Create a server
    Server s;
    SECTION("Init throws no exception") {
        CHECK_NOTHROW(s.Init(DATAPROVIDER_PORT));
    }

    // Create a 2nd server
    Server s1;
    SECTION("Init to the same port should pass") {
        // This is success because of SO_EXCLUSIVEADDRUSE
        CHECK(s1.Init(DATAPROVIDER_PORT) == true);
    }

    // Cleanup
    s.Close();
    s1.Close();

    // Clean Winsock
    CleanupWinsock();
}

/* This Test case tests the Listen() function of Server class
 */
TEST_CASE("Server Listen Function") {
    // Initialise Winsock
    InitWinsock();

    // Create server
    Server s;

    // Listen before initialisation
    SECTION("Listen before initialisation throws no exception but returns failure") {
        CHECK_NOTHROW(s.Listen() == false);
    }

    // Initialise server
    s.Init(DATAPROVIDER_PORT);

    // Listen doesn't throw exception
    SECTION("Listen throws no exception and returns success") {
        CHECK_NOTHROW(s.Listen() == true);
    }

    // Two Listen doesn't throw exception
    SECTION("Two servers can Listen at the same port together") {
        // Create server binding to the same port
        Server s1;
        s1.Init(DATAPROVIDER_PORT);
        // Listen
        CHECK_NOTHROW(s1.Listen() == true);
        // Cleanup
        s1.Close();
    }

    // Cleanup
    s.Close();

    // Clean Winsock
    CleanupWinsock();
}



/* This Test case tests the Accept() function of Server class
 */
TEST_CASE("Server Accept Function") {
    // Initialise Winsock
    InitWinsock();

    // Create server
    Server s;

    // Accept before initialisation
    SECTION("Accept before initialisation") {
        CHECK_NOTHROW(s.Accept() == INVALID_SOCKET);
    }

    // Initialise server
    s.Init(DATAPROVIDER_PORT);

    // Accept before Listen
    SECTION("Accept before Listen") {
        CHECK_NOTHROW(s.Accept() == INVALID_SOCKET);
    }

    // Start listening
    s.Listen();

    // Create a client for connection
    Client c;
    c.Connect("127.0.0.1", DATAPROVIDER_PORT);

    // Accept connection request
    SECTION("Accept connection request") {
        SOCKET clisocket;
        CHECK_NOTHROW(clisocket = s.Accept());
        CHECK(INVALID_SOCKET != clisocket);

        // cleanup
        closesocket(clisocket);
    }

    // Multiple clients connect to same server
    SECTION("Multiple clients connect to same server") {
        Client cli[10];
        SOCKET socArr[10];

        // Multiple Connects at the same time
        for (int i = 0; i < 10; ++i) {
            cli[i].Connect("127.0.0.1", DATAPROVIDER_PORT);
        }

        // Accept connections in queue
        for (int i = 0; i < 10; ++i) {
            CHECK_NOTHROW(socArr[i] = s.Accept());
        }

        // Cleanup all connections
        for (int i = 0; i < 10; ++i) {
            cli[i].Close();
            closesocket(socArr[i]);
        }
    }

    // Cleanup
    s.Close();

    // Clean Winsock
    CleanupWinsock();
}


/* This Test case tests the Send() function of Server class
 */
TEST_CASE("Server Send Function") {
    // Initialise Winsock
    InitWinsock();

    // Create a Server and a socket
    Server s;
    SOCKET oSocket(INVALID_SOCKET);

    // Server tries to send NULL data on network
    SECTION("Server send NULL data") {
        CHECK(s.Send(oSocket, NULL, 10) == true);
    }

    // Data buffer to send data out to the network
    char data[] = "ABCDEFG";
    int dataLen = 7;

    // Server tries to send data with wrong length
    SECTION("Server sends data with length 0") {
        CHECK(s.Send(oSocket, data, 0) == true);
    }

    // Server tries to send data with negative length
    SECTION("Server sends data with negative length") {
        CHECK(s.Send(oSocket, data, -1) == true);
    }

    // Server sends data on invalid socket
    SECTION("Server sends data on invalid socket") {
        CHECK(s.Send(oSocket, data, dataLen) != true);
    }

    // Initialise Server, create a client and accept it's connection
    s.Init(AGGREGATOR_PORT);
    s.Listen();
    Client c;
    c.Connect("127.0.0.1", AGGREGATOR_PORT);
    oSocket = s.Accept();

    // Server sends data successfully on socket
    SECTION("Server sends data successfully on socket") {
        CHECK(s.Send(oSocket, data, dataLen) == true);

        // Client receives data
        char buff[DEFAULT_BUFLEN] = { 0 };
        c.Receive(buff, DEFAULT_BUFLEN);
        std::string str(buff);
        CHECK(str.compare(data) == 0);
    }

    // Server sends dodgy data to Client
    SECTION("Server sends small data with huge length to Client") {
        CHECK(s.Send(oSocket, data, DEFAULT_BUFLEN) == true);

        // Client receives data
        char buff[DEFAULT_BUFLEN] = { 0 };
        c.Receive(buff, DEFAULT_BUFLEN);
        std::string str(buff);
        CHECK(str.compare(data) == 0);
    }

    // Server sends data with wrong length to Client
    SECTION("Server sends data with smaller length to Client") {
        CHECK(s.Send(oSocket, data, 2) == true);

        // Client receives data
        char buff[DEFAULT_BUFLEN] = { 0 };
        c.Receive(buff, DEFAULT_BUFLEN);
        std::string str(buff);
        CHECK(str.compare("AB") == 0);
    }

    // Cleanup Test Case
    c.Close();
    s.Close();

    // Clean Winsock
    CleanupWinsock();
}

/* This Test case tests the Receive() function of Server class
 */
TEST_CASE("Server Receive Function") {
    // Initialise Winsock
    InitWinsock();

    // Create a server
    Server s;

    // Server tries to receive data in a NULL buffer
    SECTION("Server receives data in NULL buffer") {
        CHECK(s.Receive(INVALID_SOCKET, NULL, 0) == -1);
    }

    // Data buffer to send data out to the network
    char data[DEFAULT_BUFLEN] = { 0 };

    // Buffer length 0
    SECTION("Server receives data in buffer of size 0") {
        CHECK(s.Receive(INVALID_SOCKET, data, 0) == -1);
    }

    // Receive without connection
    SECTION("Server receives data from invalid socket") {
        CHECK(s.Receive(INVALID_SOCKET, data, DEFAULT_BUFLEN) == -1);
    }

    // Initialise server
    s.Init(AGGREGATOR_PORT);
    s.Listen();
    Client c;
    c.Connect("localhost", AGGREGATOR_PORT);
    SOCKET clientSocket = s.Accept();

    // Receive data
    SECTION("Server receives data from client") {
        c.Send("ABCDEF", 6);
        CHECK(s.Receive(clientSocket, data, DEFAULT_BUFLEN) == 6);
        std::string str(data);
        CHECK(str.compare("ABCDEF") == 0);
    }

    // Cleanup sockets
    c.Close();
    s.Close();

    // Clean Winsock
    CleanupWinsock();
}

/* This Test case tests the DataProvider class
 */
TEST_CASE("DataProvider Constructor") {
    // Don't initialise Winsock, DataProvider will.

    SECTION("DataProvider Constructor initialises Winsock") {
        // Create DataProvider object - this initialises Winsock
        DataProvider d;

        // Create a client and server object
        Client c;
        Server s;

        // Winsock initialised by DataProvider constructor so the below won't fail
        CHECK(s.Init(DATAPROVIDER_PORT) ==  true);
        CHECK(s.Listen() == true);
        CHECK(c.Connect("127.0.0.1", DATAPROVIDER_PORT) == true);
        SOCKET oSocket(INVALID_SOCKET);
        CHECK((oSocket = s.Accept()) != INVALID_SOCKET);
        c.Close();
        s.Close();
    }
}

/* This Test case tests the Aggregator class
 */
TEST_CASE("Aggregator Constructor") {
    // Don't initialise Winsock, Aggregator will.

    SECTION("Aggregator Constructor initialises Winsock") {
        // Create Agggregator object - this initialises Winsock
        Aggregator d;

        // Create a client and server object for testing
        Client c;
        Server s;

        // Winsock initialised by Aggregator constructor so the below won't fail
        CHECK(s.Init(AGGREGATOR_PORT) == true);
        CHECK(s.Listen() == true);
        CHECK(c.Connect("127.0.0.1", AGGREGATOR_PORT) == true);
        SOCKET oSocket(INVALID_SOCKET);
        CHECK((oSocket = s.Accept()) != INVALID_SOCKET);
        c.Close();
        s.Close();
    }
}

/* This Test case tests the WorkerService constructor
 */
TEST_CASE("WorkerService Constructor") {
    // Don't initialise Winsock, WorkerService will.

    SECTION("WorkerService Constructor initialises Winsock") {
        // Create WorkerService object - this initialises Winsock
        WorkerService w("127.0.0.1", "127.0.0.1");

        // Create a client and server object for testing
        Client c;
        Server s;

        // Winsock initialised by WorkerService constructor so the below won't fail
        CHECK(s.Init(AGGREGATOR_PORT) == true);
        CHECK(s.Listen() == true);
        CHECK(c.Connect("127.0.0.1", AGGREGATOR_PORT) == true);
        SOCKET oSocket(INVALID_SOCKET);
        CHECK((oSocket = s.Accept()) != INVALID_SOCKET);
        c.Close();
        s.Close();
    }
}

/* This Test case tests the WorkerService Process function
 */
void DataProviderThread() {
    // Create DataProvider server simulation object
    Server sDataProvider;

    // Initialise DataProvider
    sDataProvider.Init(DATAPROVIDER_PORT);

    // DataProvider to listen for connections from WorkerService
    sDataProvider.Listen();

    // DataProvider accepts connection from WorkerService
    SOCKET oDPSocket = sDataProvider.Accept();

    // DataProvider to send GUID to WorkerService
    // Sending 16 bytes data that is same as GUID size
    sDataProvider.Send(oDPSocket, "ABCDEFGHIJKLMNOP", GUID_SIZE);

    // Cleanup
    sDataProvider.Close();
}

void AggregatorThread() {
    // Create Aggregator server simulation objects
    Server sAggregator;

    // Initialise Aggregator
    sAggregator.Init(AGGREGATOR_PORT);

    // Aggregator to listen for connections from WorkerService
    sAggregator.Listen();

    // DataProvider accepts connection from WorkerService
    SOCKET oAggSocket = sAggregator.Accept();

    // Aggregator receives data (count) from client
    char buff[DEFAULT_BUFLEN] = { 0 };
    sAggregator.Receive(oAggSocket, buff, DEFAULT_BUFLEN);

    // Cleanup
    sAggregator.Close();
}

TEST_CASE("WorkerService Process Function") {
    // Don't initialise Winsock, WorkerService will.
    WorkerService w("127.0.0.1", "127.0.0.1");

    SECTION("WorkerService Process") {
        // Create DataProvider and Aggregator simulation threads
        std::thread tDataProvider(DataProviderThread);
        std::thread tAggregator(AggregatorThread);

        // Start the Process function of WorkerService
        CHECK_NOTHROW(w.Process());

        // Wait for threads to join
        tDataProvider.join();
        tAggregator.join();
    }
}
