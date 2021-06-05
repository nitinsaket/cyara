#if !defined(__AGGREGATOR_H__)
#define __AGGREGATOR_H__

#include <iostream>
#include "Server.h"
#include <list>
#include <mutex>

// Struct to securely access count list
struct CountList {
    // Digit Count List
    std::list<int> m_CountList;

    // Mutex
    std::mutex m_Mutex;
};

// Aggregator class
class Aggregator {
public:
    // Constructors and Destructors
    Aggregator();
    ~Aggregator();

    // Process function
    void Process();

private:
    // Server object
    Server m_Server;

    // Count List struct
    CountList m_CountStruct;

};  // Aggregator

#endif