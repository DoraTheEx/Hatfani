#include <windows.h>
#include <stdio.h>

int main() {
    HKEY hKey;
    LONG lResult = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.doc\\OpenWithList", 0, KEY_WRITE, &hKey);

    if (lResult == ERROR_SUCCESS) {
        lResult = RegSetValueEx(hKey, "", 0, REG_SZ, "calc.exe", sizeof("calc.exe"));
        RegCloseKey(hKey);

        if (lResult == ERROR_SUCCESS) {
            printf("Registry modified successfully.\n");
            return 0;
        } else {
            printf("Failed to set registry value.\n");
            return 1;
        }
    } else {
        printf("Failed to open registry key.\n");
        return 1;
    }
}
