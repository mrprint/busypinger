#define WINVER _WIN32_WINNT_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <windows.h>
#include <wtsapi32.h>
#include <sstream>
#include "payload.hpp"

template<typename R>
auto use_enum_session(bool(*func)(R&, const _WTS_SESSION_INFOA&)) -> R
{
    using namespace std;

    PWTS_SESSION_INFOA pinfo=nullptr;
    DWORD count=0;
    R result = R{};
    auto senumer = RAIInplace(
        [&](){
            if (!WTSEnumerateSessionsA(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pinfo, &count)) {
                throw std::system_error(
                    EINTR, std::generic_category(), "sessions enumeration failure"
                );
            }
        },
        [&](){ WTSFreeMemory(pinfo); }
    );

    for (size_t i=0; i < count; ++i)
    {
        if (std::string::npos != std::string(pinfo[i].pWinStationName).find("RDP-Tcp#")
                && WTSActive == pinfo[i].State) {
            if (func(result, pinfo[i]))
                break;
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////

bool isbusy()
{
    return use_enum_session(
        +[](bool& result, const _WTS_SESSION_INFOA& info) {
            result = true;
            return true;
        }
    );
}

static VectorStr _ips_get()
{
    return use_enum_session(
        +[](VectorStr& result, const _WTS_SESSION_INFOA& info){
            LPSTR ppBuffer=nullptr;
            DWORD pBytesReturned=0;
            auto sienumer = RAIInplace(
                [&](){
                    if (!WTSQuerySessionInformationA(
                                WTS_CURRENT_SERVER_HANDLE,
                                info.SessionId,
                                WTSClientAddress,
                                &ppBuffer,
                                &pBytesReturned)) {
                        result.push_back("ERROR");
                    }
                },
                [&](){ WTSFreeMemory(ppBuffer); }
            );
            if (auto wtsca = reinterpret_cast<WTS_CLIENT_ADDRESS*>(ppBuffer); wtsca->AddressFamily == AF_INET) {
                std::stringstream ss;
                ss
                        << std::to_string(wtsca->Address[2])
                        << "." << std::to_string(wtsca->Address[3])
                        << "." << std::to_string(wtsca->Address[4])
                        << "." << std::to_string(wtsca->Address[5]);
                result.push_back(ss.str());
            } else {
                result.push_back("UNSUPPORTED PROTOCOL");
            }
            return false;
        }
    );
}

static VectorStr _names_get()
{
    return use_enum_session(
        +[](VectorStr& result, const _WTS_SESSION_INFOA& info){
            LPSTR ppBuffer=nullptr;
            DWORD pBytesReturned=0;
            auto sienumer = RAIInplace(
                [&](){
                    if (!WTSQuerySessionInformationA(
                                WTS_CURRENT_SERVER_HANDLE,
                                info.SessionId,
                                WTSClientName,
                                &ppBuffer,
                                &pBytesReturned)) {
                        result.push_back("ERROR");
                    }
                },
                [&](){ WTSFreeMemory(ppBuffer); }
            );
            result.push_back(ppBuffer);
            return false;
        }
    );
}


VectorStr ips_get()
{
    VectorStr result;
    if (isbusy()) {
        result = _ips_get();
    }
    return result;
}

VectorStr names_get()
{
    VectorStr result;
    if (isbusy()) {
        result = _names_get();
    }
    return result;
}
