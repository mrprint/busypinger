#include "service_base.hpp"
#include <cassert>

ServiceBase* ServiceBase::m_service = nullptr;

ServiceBase::ServiceBase(const std::wstring& name,
                         const std::wstring& displayName,
                         DWORD dwStartType,
                         DWORD dwErrCtrlType,
                         DWORD dwAcceptedCmds,
                         const std::wstring& depends,
                         const std::wstring& account,
                         const std::wstring& password)
    : m_name(name),
      m_displayName(displayName),
      m_dwStartType(dwStartType),
      m_dwErrorCtrlType(dwErrCtrlType),
      m_depends(depends),
      m_account(account),
      m_password(password),
      m_svcStatusHandle(nullptr) {

    m_svcStatus.dwControlsAccepted = dwAcceptedCmds;
    m_svcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_svcStatus.dwCurrentState = SERVICE_START_PENDING;
    m_svcStatus.dwWin32ExitCode = NO_ERROR;
    m_svcStatus.dwServiceSpecificExitCode = 0;
    m_svcStatus.dwCheckPoint = 0;
    m_svcStatus.dwWaitHint = 0;
}

void ServiceBase::SetStatus(DWORD dwState, DWORD dwErrCode, DWORD dwWait) {
    m_svcStatus.dwCurrentState = dwState;
    m_svcStatus.dwWin32ExitCode = dwErrCode;
    m_svcStatus.dwWaitHint = dwWait;

    ::SetServiceStatus(m_svcStatusHandle, &m_svcStatus);
}

void ServiceBase::WriteToEventLog(const std::wstring& msg, WORD type) {
    HANDLE hSource = RegisterEventSourceW(nullptr, m_name.c_str());
    if (hSource) {
        const WCHAR* msgData[2] = {m_name.c_str(), msg.c_str()};
        ReportEventW(hSource, type, 0, 0, nullptr, 2, 0, msgData, nullptr);
        DeregisterEventSource(hSource);
    }
}

// static
void WINAPI ServiceBase::svcMain(DWORD argc, WCHAR* argv[]) {
    assert(m_service);

    m_service->m_svcStatusHandle = ::RegisterServiceCtrlHandlerExW(m_service->getName().c_str(),
                                                                   serviceCtrlHandler, nullptr);
    if (!m_service->m_svcStatusHandle) {
        m_service->WriteToEventLog(L"Can't set service control handler",
                                   EVENTLOG_ERROR_TYPE);
        return;
    }

    m_service->start(argc, argv);
}

// static
DWORD WINAPI ServiceBase::serviceCtrlHandler(DWORD ctrlCode, DWORD evtType,
                                             void* evtData, void* /*context*/) {
    switch (ctrlCode) {
    case SERVICE_CONTROL_STOP:
        m_service->stop();
        break;

    case SERVICE_CONTROL_PAUSE:
        m_service->pause();
        break;

    case SERVICE_CONTROL_CONTINUE:
        m_service->continue_();
        break;

    case SERVICE_CONTROL_SHUTDOWN:
        m_service->shutdown();
        break;

    case SERVICE_CONTROL_SESSIONCHANGE:
        m_service->onSessionChange(evtType, reinterpret_cast<WTSSESSION_NOTIFICATION*>(evtData));
        break;

    default:
        break;
    }

    return 0;
}

bool ServiceBase::runInternal(ServiceBase* svc) {
    m_service = svc;

    WCHAR* svcName = const_cast<WCHAR*>(m_service->getName().c_str());

    SERVICE_TABLE_ENTRYW tableEntry[] = {
        {svcName, svcMain},
        {nullptr, nullptr}
    };

    return ::StartServiceCtrlDispatcherW(tableEntry) == TRUE;
}

void ServiceBase::start(DWORD argc, WCHAR* argv[]) {
    SetStatus(SERVICE_START_PENDING);
    onStart(argc, argv);
    SetStatus(SERVICE_RUNNING);
}

void ServiceBase::stop() {
    SetStatus(SERVICE_STOP_PENDING);
    onStop();
    SetStatus(SERVICE_STOPPED);
}

void ServiceBase::pause() {
    SetStatus(SERVICE_PAUSE_PENDING);
    onPause();
    SetStatus(SERVICE_PAUSED);
}

void ServiceBase::continue_() {
    SetStatus(SERVICE_CONTINUE_PENDING);
    onContinue();
    SetStatus(SERVICE_RUNNING);
}

void ServiceBase::shutdown() {
    onShutdown();
    SetStatus(SERVICE_STOPPED);
}
