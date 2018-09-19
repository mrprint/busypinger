#include <windows.h>
#include <wtsapi32.h>
#include <string>
#include <iostream>
#include <system_error>
#include "payload.hpp"

struct WTSEnumerateSessionsC {
    // RAII
    PWTS_SESSION_INFOA pinfo=nullptr;
    DWORD count;
    WTSEnumerateSessionsC():
        pinfo(nullptr),
        count(0)
    {
        if (!WTSEnumerateSessionsA(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pinfo, &count)) {
            throw std::system_error(EINTR, std::generic_category(), "sessions enumeration failure");
        }
    }
    ~WTSEnumerateSessionsC()
    {
        WTSFreeMemory(pinfo);
    }
};

bool isbusy()
{
    using namespace std;

    WTSEnumerateSessionsC senumer;
    for (size_t i=0; i < senumer.count; ++i)
    {
        if (string::npos != string(senumer.pinfo[i].pWinStationName).find("RDP-Tcp#")
                && WTSActive == senumer.pinfo[i].State) {
            return true;
        }
    }
    return false;
}

