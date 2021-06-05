#include <iostream>
#include "WorkerService.h"

// Main program
int main(int argc, char** argv) {
    // IP Address of the servers
    const char* pDataProviderAddr = "localhost";
    const char* pAggregatorAddr = "localhost";

    // If IP address of both servers is provided at cmdline
    if (argc == 3) {
        pDataProviderAddr = argv[1];
        pAggregatorAddr = argv[2];
    }
    else if (argc > 1) {
        LOG("Usage:");
        LOG("1.  WorkerService.exe");
        LOG("2.  WorkerService.exe <Data-Provider's IP Address> <Aggregator Service's IP Address>");
        LOG("");
        LOG("Note: Usage 1 assumes servers are on localhost");
        LOG("Note: Default behaviour is Usage 1 if invalid command line arguments are provided.");
        LOG("\n\n");
    }

    try {
        // Start Worker Service
        WorkerService w(pDataProviderAddr, pAggregatorAddr);
        w.Process();
    }
    catch (std::exception &e) {
        LOG("Exception: " << e.what());
        LOG("Exiting Process");
    }

    // Exit
    return 0;
}