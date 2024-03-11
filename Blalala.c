#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

void listProcesses();
void processDetails(DWORD pid);
void killProcess(DWORD pid);

int main() {
    int choice;
    DWORD pid;

    do {
        printf("\n1. List running processes\n");
        printf("2. Show details for a specific process\n");
        printf("3. Kill a process\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                listProcesses();
                break;
            case 2:
                printf("Enter the PID of the process: ");
                scanf("%lu", &pid);
                processDetails(pid);
                break;
            case 3:
                printf("Enter the PID of the process to kill: ");
                scanf("%lu", &pid);
                killProcess(pid);
                break;
            case 4:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);

    return 0;
}

void listProcesses() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &processEntry)) {
        do {
            printf("PID: %lu, Process Name: %s\n", processEntry.th32ProcessID, processEntry.szExeFile);
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
}

void processDetails(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process != NULL) {
        // Additional details can be retrieved here based on your needs
        printf("Details for Process with PID %lu\n", pid);
        CloseHandle(process);
    } else {
        printf("Error opening process. Please check the PID and try again.\n");
    }
}

void killProcess(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (process != NULL) {
        if (TerminateProcess(process, 0)) {
            printf("Process with PID %lu terminated successfully.\n", pid);
        } else {
            printf("Error terminating process. Please check the PID and try again.\n");
        }
        CloseHandle(process);
    } else {
        printf("Error opening process. Please check the PID and try again.\n");
    }
}
