#include <Windows.h>
#include <iostream>

// Function prototype for the service worker function
VOID WINAPI ServiceWorker(DWORD dwArgc, LPTSTR* lpszArgv);
// Entry point for the service
int main()
{
    // Define service name and display name
    LPCTSTR serviceName = TEXT("SimpleService");
    LPCTSTR displayName = TEXT("Simple Service Example");

    // Register the service control handler
    SERVICE_TABLE_ENTRY serviceTable[] =
    {
        { (LPWSTR)serviceName, (LPSERVICE_MAIN_FUNCTION)ServiceWorker },
        { NULL, NULL }
    };

    // Start the service control dispatcher
    if (!StartServiceCtrlDispatcher(serviceTable))
    {
        std::cerr << "Failed to start service control dispatcher" << std::endl;
        return GetLastError();
    }

    return 0;
}

// Service worker function
VOID WINAPI ServiceWorker(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // Register the service control handler
    SERVICE_STATUS_HANDLE serviceStatusHandle = RegisterServiceCtrlHandler(lpszArgv[0], NULL);
    if (serviceStatusHandle == NULL)
    {
        std::cerr << "Failed to register service control handler" << std::endl;
        return;
    }

    // Set service status to running
    SERVICE_STATUS serviceStatus;
    ZeroMemory(&serviceStatus, sizeof(serviceStatus));
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    serviceStatus.dwWin32ExitCode = NO_ERROR;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;

    SetServiceStatus(serviceStatusHandle, &serviceStatus);

    // Main service loop
    while (serviceStatus.dwCurrentState == SERVICE_RUNNING)
    {
        // Perform service tasks here
        Sleep(1000); // Placeholder for actual service logic
    }

    // Set service status to stopped
    serviceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(serviceStatusHandle, &serviceStatus);
}
