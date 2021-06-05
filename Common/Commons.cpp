#include <iostream>
#include "Commons.h"

/* Overload operator << to extract GUID
 */
std::ostringstream& operator<<(std::ostringstream& os, REFGUID guid) {

    os << std::uppercase;
    os.width(8);
    os << std::hex << guid.Data1 << '-';

    os.width(4);
    os << std::hex << guid.Data2 << '-';

    os.width(4);
    os << std::hex << guid.Data3 << '-';

    os.width(2);
    os << std::hex
        << static_cast<short>(guid.Data4[0])
        << static_cast<short>(guid.Data4[1])
        << '-'
        << static_cast<short>(guid.Data4[2])
        << static_cast<short>(guid.Data4[3])
        << static_cast<short>(guid.Data4[4])
        << static_cast<short>(guid.Data4[5])
        << static_cast<short>(guid.Data4[6])
        << static_cast<short>(guid.Data4[7]);
    os << std::nouppercase;
    return os;
}   // operator<<
// *********************************************************************************

/* Convert int between 0 - 15 to Hex character
 */
char HexChar(int l_Byte) {
    if (l_Byte <= 9) {
        return '0' + l_Byte;
    }
    else {
        return 'A' + (l_Byte - 10);
    }
}   // HexChar
// *********************************************************************************

/* Convert a single byte to 2 HEX charcaters
 */
void ByteToHEX(unsigned char l_Char, char* l_pBuff) {
    int byte1(l_Char / 16);
    int byte2(l_Char % 16);

    l_pBuff[0] = HexChar(byte1);
    l_pBuff[1] = HexChar(byte2);
}   // ByteToHEX
// *********************************************************************************

/* Convert GUID to HEX character string
 * Note: This function does not give the HEX characters in order
 *       for Data1, Data2 and Data3 of GUID - BUT - we can still
 *       use it to correctly count the digits in a GUID
 */
void GUIDToHex(GUID& l_GUID, char* l_Dest, int l_lenDest) {
    // Initialise destination buffer with NULL
    memset(l_Dest, 0, l_lenDest);

    // Convert the GUID byte-by-byte to HEX characters
    unsigned char* data = (unsigned char*)&l_GUID;
    for (int i(0); i < 16; ++i) {
        ByteToHEX(data[i], &l_Dest[i * 2]);
    }
}   // GUIDToHex
// *********************************************************************************