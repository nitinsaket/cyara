#include <iostream>
#include "WorkerService.h"

// Main program
int main() {
    try {
        // Start Worker Service
        WorkerService w;
        w.Process();
    }
    catch (std::exception &e) {
        LOG("Exception: " << e.what());
        LOG("Exiting Process");
    }

    // Exit
    return 0;
}