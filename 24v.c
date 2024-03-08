#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 24

int main(int argc, char *argv[]) {
    HANDLE hFile, hNewFile;
    DWORD dwBytesRead, dwBytesWritten;
    char buffer[BUFFER_SIZE];
    // Open input file
    hFile = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Error opening input file\n");
        return 1;
    }
    // Read first 24 bytes
    if (!ReadFile(hFile, buffer, BUFFER_SIZE, &dwBytesRead, NULL)) {
        printf("Error reading from input file\n");
        CloseHandle(hFile);
        return 1;
    }
    // Close input file
    CloseHandle(hFile);
    // Create new file
    hNewFile = CreateFile("output.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hNewFile == INVALID_HANDLE_VALUE) {
        printf("Error creating output file\n");
        return 1;
    }
    // Write to new file
    if (!WriteFile(hNewFile, buffer, dwBytesRead, &dwBytesWritten, NULL) || dwBytesWritten != dwBytesRead) {
        printf("Error writing to output file\n");
        CloseHandle(hNewFile);
        return 1;
    }
    // Close new file
    CloseHandle(hNewFile);
    return 0;
}
