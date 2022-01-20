// from: https://stackoverflow.com/a/4548854/12291425

#ifdef UNICODE
#undef UNICODE
#endif
#include <Windows.h>
#include <Tlhelp32.h>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <thread>
#include <chrono>
using namespace std;

// from: https://stackoverflow.com/a/32770084/12291425
BOOL TerminateMyProcess(DWORD dwProcessId, UINT uExitCode)
{
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL bInheritHandle = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    if (hProcess == NULL)
        return FALSE;

    BOOL result = TerminateProcess(hProcess, uExitCode);

    CloseHandle(hProcess);

    return result;
}

void find_process(string sAppPath, string sAppName)
{
    PROCESSENTRY32 pe32 = {0};
    HANDLE hSnap;
    int iDone;
    bool bProcessFound;

    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe32.dwSize = sizeof(PROCESSENTRY32);
    Process32First(hSnap, &pe32); // Can throw away, never an actual app

    bProcessFound = false; //init values
    iDone = 1;
    DWORD dwProcessID = 0;
    while (iDone) // go until out of Processes
    {
        iDone = Process32Next(hSnap, &pe32);
        string s{pe32.szExeFile};
        if (boost::starts_with(boost::to_lower_copy(s), "notepad.exe"))
        {
            iDone = 0;
            bProcessFound = true;
            dwProcessID = pe32.th32ProcessID;
        }
    }

    if (!bProcessFound) // if we didn't find it running...
    {
        // additional information
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        // set the size of the structures
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        CreateProcess((sAppPath + sAppName).c_str(),
                      NULL,
                      NULL,
                      NULL, FALSE, 0, NULL, NULL, &si, &pi); // start it
        dwProcessID = pi.dwProcessId;

    }

    this_thread::sleep_for(5s);
    TerminateMyProcess(dwProcessID, 1);
}

int main()
{
    string appPath{"C:\\Windows\\"};
    string appName{"notepad.exe"};
    find_process(appPath, appName);

    return 0;
}