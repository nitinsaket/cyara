#if !defined(__DATAPROVIDER_H__)
#define __DATAPROVIDER_H__

#include <iostream>
#include "Server.h"
#include "combaseapi.h"

// Data Provider
class DataProvider {
public:
    // Constructor and Destructor
    DataProvider();
    ~DataProvider();

    // Process function
    void Process();

private:
    // Server object
    Server m_Server;

    // Function to generate a GUID
    bool __GenerateGUID(GUID& l_GUID);

    // Send GUID to client
    bool __SendGUID(SOCKET l_Client, const GUID& l_GUID);

};  // DataProvider

#endif