#include <iostream>
#include "Aggregator.h"

// Main Program
int main() {
    try {
        // Start Aggregator Service
        Aggregator a;
        a.Process();
    }
    catch (std::exception &e) {
        LOG("Exception: " << e.what());
        LOG("Exiting Process");
    }

    return 0;
}