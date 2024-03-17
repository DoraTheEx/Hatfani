#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

void ListProcessModules(DWORD dwPID)
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        printf("CreateToolhelp32Snapshot (of modules) failed.\n");
        return;
    }

    // Set the size of the structure before using it.
    me32.dwSize = sizeof(MODULEENTRY32);

    // Retrieve information about the first module,
    // and exit if unsuccessful.
    if (!Module32First(hModuleSnap, &me32))
    {
        printf("Module32First failed.\n"); // No modules found.
        CloseHandle(hModuleSnap);          // Clean the snapshot object.
        return;
    }

    // Now walk the module list of the process and
    // display information about each module.
    printf("\nDLLs loaded by process %lu:\n", dwPID);
    do
    {
        printf("\t%s\n", me32.szModule);
    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
}

int main()
{
    DWORD dwPID;
    printf("Enter the PID of the process: ");
    scanf("%lu", &dwPID);

    ListProcessModules(dwPID);
    return 0;
}
