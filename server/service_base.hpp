#ifndef SERVICE_BASE_HPP_
#define SERVICE_BASE_HPP_

#include <windows.h>
#include <string>

// Base Service class used to create windows services.
class ServiceBase {
public:
    ServiceBase(const ServiceBase& other) = delete;
    ServiceBase& operator=(const ServiceBase& other) = delete;

    ServiceBase(ServiceBase&& other) = delete;
    ServiceBase& operator=(ServiceBase&& other) = delete;

    virtual ~ServiceBase() {}

    // Called by windows when starting the service.
    bool run() {
        return runInternal(this);
    }

    const std::wstring& getName() const { return m_name; }
    const std::wstring& getDisplayName() const { return m_displayName; }
    DWORD getStartType() const { return m_dwStartType; }
    DWORD getErrorControlType() const { return m_dwErrorCtrlType; }
    const std::wstring& getDependencies() const { return m_depends; }

    // Account info service runs under.
    const std::wstring& getAccount() const { return m_account; }
    const std::wstring& getPassword() const { return m_password; }
protected:
    ServiceBase(const std::wstring& name,
                const std::wstring& displayName,
                DWORD dwStartType,
                DWORD dwErrCtrlType = SERVICE_ERROR_NORMAL,
                DWORD dwAcceptedCmds = SERVICE_ACCEPT_STOP,
                const std::wstring& depends = L"",
                const std::wstring& account = L"",
                const std::wstring& password = L"");

    void SetStatus(DWORD dwState, DWORD dwErrCode = NO_ERROR, DWORD dwWait = 0);

    // TODO(Olster): Move out of class/make static.
    // Writes |msg| to Windows event log.
    void WriteToEventLog(const std::wstring& msg, WORD type = EVENTLOG_INFORMATION_TYPE);

    // Overro=ide these functions as you need.
    virtual void onStart(DWORD argc, WCHAR* argv[]) = 0;
    virtual void onStop() {}
    virtual void onPause() {}
    virtual void onContinue() {}
    virtual void onShutdown() {}

    virtual void onSessionChange(DWORD /*evtType*/,
                                 WTSSESSION_NOTIFICATION* /*notification*/) {}
private:
    // Registers handle and starts the service.
    static void WINAPI svcMain(DWORD argc, WCHAR* argv[]);

    // Called whenever service control manager updates service status.
    static DWORD WINAPI serviceCtrlHandler(DWORD ctrlCode, DWORD evtType,
                                           void* evtData, void* context);

    static bool runInternal(ServiceBase* svc);

    void start(DWORD argc, WCHAR* argv[]);
    void stop();
    void pause();
    void continue_();
    void shutdown();

    std::wstring m_name;
    std::wstring m_displayName;
    DWORD m_dwStartType;
    DWORD m_dwErrorCtrlType;
    std::wstring m_depends;
    std::wstring m_account;
    std::wstring m_password;

    // Info about service dependencies and user account.
    bool m_hasDepends = false;
    bool m_hasAcc = false;
    bool m_hasPass = false;

    SERVICE_STATUS m_svcStatus;
    SERVICE_STATUS_HANDLE m_svcStatusHandle;

    static ServiceBase* m_service;
};

#endif // SERVICE_BASE_HPP_
