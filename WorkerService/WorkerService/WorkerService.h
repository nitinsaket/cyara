#if !defined(__WORKERSERVICE_H__)
#define __WORKERSERVICE_H__

#include <iostream>
#include "Client.h"

// Worker Service
class WorkerService {
public:
    // Constructor and Destructor
    WorkerService();
    ~WorkerService();

    // Worker Service's functionality
    void Process();

private:
    // Client connecting to Data Provider Service
    Client m_DataProviderClient;

    // Client connecting to Aggregator Service
    Client m_AggregatorClient;

    // Count digits in GUID
    int __CountDigitsInGUID(const char* l_pGUIDBuff);

};  // WorkerService

#endif