#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

// Function to print details of a specified process identified by PID
void printProcessDetails(DWORD pid);

int main() {
    DWORD pid;

    // Input PID from user
    printf("Enter the PID of the process: ");
    scanf("%lu", &pid);

    // Call function to print process details
    printProcessDetails(pid);

    return 0;
}

// Function to print details of a specified process identified by PID
void printProcessDetails(DWORD pid) {
    // Open the process with required access rights
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (process != NULL) {
        // Get process image path
        char filePath[MAX_PATH];
        if (GetModuleFileNameExA(process, NULL, filePath, MAX_PATH) > 0) {
            printf("Process Image Path: %s\n", filePath);
        } else {
            printf("Error retrieving process image path.\n");
        }

        // Get process user
        HANDLE token;
        if (OpenProcessToken(process, TOKEN_QUERY, &token)) {
            DWORD tokenInfoLength;
            // Retrieve token information length
            if (GetTokenInformation(token, TokenUser, NULL, 0, &tokenInfoLength) == FALSE) {
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                    // Allocate memory for token information
                    PTOKEN_USER tokenUser = (PTOKEN_USER)malloc(tokenInfoLength);
                    if (GetTokenInformation(token, TokenUser, tokenUser, tokenInfoLength, &tokenInfoLength)) {
                        SID_NAME_USE sidNameUse;
                        char userName[MAX_PATH];
                        DWORD userNameSize = MAX_PATH;
                        char domainName[MAX_PATH];
                        DWORD domainNameSize = MAX_PATH;

                        // Lookup account SID to get user information
                        if (LookupAccountSidA(NULL, tokenUser->User.Sid, userName, &userNameSize, domainName, &domainNameSize, &sidNameUse)) {
                            printf("User: %s\\%s\n", domainName, userName);
                        } else {
                            printf("Error looking up account SID.\n");
                        }
                    } else {
                        printf("Error getting token information.\n");
                    }
                    free(tokenUser);
                } else {
                    printf("Error getting token information length.\n");
                }
            } else {
                printf("Error getting token information (unexpected).\n");
            }
            CloseHandle(token);
        } else {
            printf("Error opening process token.\n");
        }

        // Get process parent PID
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (Process32First(snapshot, &processEntry)) {
            do {
                if (processEntry.th32ProcessID == pid) {
                    printf("Parent PID: %lu\n", processEntry.th32ParentProcessID);
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        } else {
            printf("Error getting process snapshot.\n");
        }

        CloseHandle(snapshot);

        // Get process threads
        printf("Thread IDs:\n");
        HANDLE snapshotThreads = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        THREADENTRY32 threadEntry;
        threadEntry.dwSize = sizeof(THREADENTRY32);

        if (Thread32First(snapshotThreads, &threadEntry)) {
            do {
                if (threadEntry.th32OwnerProcessID == pid) {
                    printf("- %lu\n", threadEntry.th32ThreadID);
                }
            } while (Thread32Next(snapshotThreads, &threadEntry));
        } else {
            printf("Error getting thread snapshot.\n");
        }

        CloseHandle(snapshotThreads);

        // Close the process handle
        CloseHandle(process);
    } else {
        printf("Error opening process. Please check the PID and try again.\n");
    }
}
