#define INITGUID
#include "installer.h"
#include <shellapi.h>
#include <string.h>
#include <shlobj.h>
#include <objbase.h>

void AddToStartup() {
    HKEY hKey;
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "ShowDesktopApp", 0, REG_SZ, (unsigned char*)path, strlen(path) + 1);
        RegCloseKey(hKey);
    }
}

void RemoveFromStartup() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegDeleteValueA(hKey, "ShowDesktopApp");
        RegCloseKey(hKey);
    }
}

void InstallToProgramFiles() {
    char currentPath[MAX_PATH];
    char programFiles[MAX_PATH];
    char targetDir[MAX_PATH];
    char targetPath[MAX_PATH];
    char startMenuPath[MAX_PATH];
    char shortcutFolder[MAX_PATH];
    char shortcutPath[MAX_PATH];

    GetModuleFileNameA(NULL, currentPath, MAX_PATH);
    if (GetEnvironmentVariableA("ProgramFiles", programFiles, MAX_PATH) == 0) {
        return;
    }

    wsprintfA(targetDir, "%s\\DesktopHelper", programFiles);
    CreateDirectoryA(targetDir, NULL);
    wsprintfA(targetPath, "%s\\ShowDesktop.exe", targetDir);

    if (!CopyFileA(currentPath, targetPath, FALSE)) {
        MessageBoxA(NULL, "Failed to copy executable to Program Files.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "ShowDesktopApp", 0, REG_SZ, (unsigned char*)targetPath, strlen(targetPath) + 1);
        RegCloseKey(hKey);
    }

    if (SHGetSpecialFolderPathA(NULL, startMenuPath, CSIDL_COMMON_PROGRAMS, FALSE)) {
        wsprintfA(shortcutFolder, "%s\\DesktopHelper", startMenuPath);
        CreateDirectoryA(shortcutFolder, NULL);
        wsprintfA(shortcutPath, "%s\\Minimize All.lnk", shortcutFolder);

        CoInitialize(NULL);
        IShellLinkA* psl;
        if (SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLinkA, (void**)&psl))) {
            psl->lpVtbl->SetPath(psl, targetPath);
            psl->lpVtbl->SetDescription(psl, "Show Desktop");
            IPersistFile* ppf;
            if (SUCCEEDED(psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (void**)&ppf))) {
                WCHAR wsz[MAX_PATH];
                MultiByteToWideChar(CP_ACP, 0, shortcutPath, -1, wsz, MAX_PATH);
                ppf->lpVtbl->Save(ppf, wsz, TRUE);
                ppf->lpVtbl->Release(ppf);
            }
            psl->lpVtbl->Release(psl);
        }
        CoUninitialize();
    }

    MessageBoxA(NULL, "Sucessfully Installed", "Success", MB_OK | MB_ICONINFORMATION);
}
