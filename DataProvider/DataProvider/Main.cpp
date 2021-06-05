#include <iostream>
#include "DataProvider.h"

// Main Program
int main() {
    try {
        // Start Data Provider
        DataProvider d;
        d.Process();
    }
    catch (std::exception &e) {
        LOG("Exception: " << e.what());
        LOG("Exiting Process");
    }

    // Exit
    return 0;
}