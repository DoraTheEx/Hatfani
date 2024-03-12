#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

#define SECRET_KEY "ObfuscatedValue" // Key for function name encryption (replace with your own)

// Function to encrypt function names using XOR with a secret key
char* encryptFunctionName(const char* name) {
    int len = strlen(name);
    char* encryptedName = (char*)malloc(len + 1);
    for (int i = 0; i < len; i++) {
        encryptedName[i] = name[i] ^ SECRET_KEY[i % strlen(SECRET_KEY)];
    }
    encryptedName[len] = '\0';
    return encryptedName;
}

// Function to decrypt function names using XOR with the secret key
char* decryptFunctionName(const char* name) {
    int len = strlen(name);
    char* decryptedName = (char*)malloc(len + 1);
    for (int i = 0; i < len; i++) {
        decryptedName[i] = name[i] ^ SECRET_KEY[i % strlen(SECRET_KEY)];
    }
    decryptedName[len] = '\0';
    return decryptedName;
}

// Function to print details of a specified process identified by PID
void obfuscated_printProcessDetails(DWORD pid) {
    // Open the process with required access rights
    HANDLE obfuscated_OpenProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (obfuscated_OpenProcess != NULL) {
        // Get process image path
        char filePath[MAX_PATH];
        if (GetModuleFileNameExA(obfuscated_OpenProcess, NULL, filePath, MAX_PATH) > 0) {
            printf("Process Image Path: %s\n", filePath);
        } else {
            printf("Error retrieving process image path.\n");
        }

        // Get process user
        HANDLE token;
        if (OpenProcessToken(obfuscated_OpenProcess, TOKEN_QUERY, &token)) {
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

