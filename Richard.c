#include <windows.h>
#include <stdio.h>

void ListProcessModules(DWORD dwPID)
{
    HANDLE hProcess;
    HMODULE hMods[1024];
    DWORD cbNeeded;

    // Get a handle to the process.
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
    if (NULL == hProcess)
        return;

    // Get a list of all the modules in this process.
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
            {
                // Print the module name.
                _tprintf(TEXT("%s\n"), szModName);
            }
        }
    }

    // Release the handle to the process.
    CloseHandle(hProcess);
}

int main()
{
    DWORD dwPID;
    printf("Enter the PID of the process: ");
    scanf("%lu", &dwPID);

    ListProcessModules(dwPID);
    return 0;
}
