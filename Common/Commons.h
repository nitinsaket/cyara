#if !defined(__COMMONS_H__)
#define __COMMONS_H__

/*
 * This file holds functions, macros and declarations common to all components
 */

#include <sstream>
#include "combaseapi.h"

// Print GUID
#define PRINTGUID(guid) \
    wchar_t sDest[100]; \
    StringFromGUID2(guid, sDest, 100); \
    std::wcout << "GUID: " << sDest << std::endl;

// Boolean macros
#define SUCCESS     true
#define FAILURE     false

// Listener Port for Data Provider Service
#define DATAPROVIDER_PORT   "7777"

// Listener Port for Aggregator Service
#define AGGREGATOR_PORT     "8888"

// Default buffer length
#define DEFAULT_BUFLEN      512

// Size of GUID structure
#define GUID_SIZE           sizeof(GUID)

/* Number of characters in a HEX representation of GUID (as shown below including '{' and '}')
 * {FE6DE3B5-F63C-4A79-BEFA-F1E4E3A2D2D8} = 38
 */
#define GUIDSTRLEN          38

// Comment/Uncomment the immediate next line to disable/enable debug logs
//#define DISABLE_LOGS
#if defined(DISABLE_LOGS)
    // Debug Log disabled
    #define DEBUGLOG(msg)
#else
    // Debug Log
    #define DEBUGLOG(msg)  \
        std::cout << msg << std::endl;
        // std::cout << msg << "(" << __FILE__ << ":" << __LINE__ << ")" << std::endl;
#endif

// Priority Log - this is for important logs
#define LOG(msg)    \
    std::cout << msg << std::endl;
    // std::cout << msg << "(" << __FILE__ << ":" << __LINE__ << ")" << std::endl;

#endif