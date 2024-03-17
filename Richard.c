#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

void PrintProcessModules(DWORD processID) {
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot (of modules) failed\n");
        return;
    }

    // Set the size of the structure before using it.
    me32.dwSize = sizeof(MODULEENTRY32);

    // Retrieve information about the first module,
    // and exit if unsuccessful
    if (!Module32First(hModuleSnap, &me32)) {
        printf("Module32First failed\n");
        CloseHandle(hModuleSnap);     // Must clean up the snapshot object!
        return;
    }

    // Now walk the module list of the process
    do {
        printf("Module name: %s\n", me32.szModule);
    } while (Module32Next(hModuleSnap, &me32));

    // Do not forget to clean up the snapshot object.
    CloseHandle(hModuleSnap);
}

int main() {
    DWORD processID;
    printf("Enter the process ID: ");
    scanf("%lu", &processID);

    PrintProcessModules(processID);

    return 0;
}
