// from: https://stackoverflow.com/a/4548854/12291425

#ifdef UNICODE
#undef UNICODE
#endif
#include <Windows.h>
#include <Tlhelp32.h>
#include <string>
using namespace std;

void find_process(string sAppPath, string sAppName)
{
    PROCESSENTRY32 pe32 = {0};
    HANDLE hSnap;
    int iDone;
    int iTime = 60;
    bool bProcessFound;

    while (true) // go forever
    {
        hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        pe32.dwSize = sizeof(PROCESSENTRY32);
        Process32First(hSnap, &pe32); // Can throw away, never an actual app

        bProcessFound = false; //init values
        iDone = 1;

        while (iDone) // go until out of Processes
        {
            iDone = Process32Next(hSnap, &pe32);
            if (pe32.szExeFile == sAppName) // Did we find our process?
            {
                bProcessFound = true;
                iDone = 0;
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
        }
        Sleep(iTime * 1000); // delay x amount of seconds.
    }
}

int main()
{
    string appPath{"C:\\Windows\\"};
    string appName{"notepad.exe"};
    find_process(appPath, appName);
    return 0;
}