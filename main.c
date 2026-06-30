#include <windows.h>
#include <shellapi.h>
#include <string.h>
#include <shlobj.h>
#include "installer.h"

#define ID_TRAY_ICON 1001
#define WM_TRAYICON (WM_USER + 1)
#define IDM_ADD_STARTUP 2001
#define IDM_REM_STARTUP 2002
#define IDM_EXIT 2003
#define IDM_INSTALL 2004

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (IsWindowVisible(hwnd)) {
        char className[256];
        GetClassNameA(hwnd, className, sizeof(className));
        if (strcmp(className, "Progman") == 0 ||
            strcmp(className, "WorkerW") == 0 ||
            strcmp(className, "Shell_TrayWnd") == 0 ||
            strcmp(className, "Shell_SecondaryTrayWnd") == 0) {
            return TRUE;
        }
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        if ((exStyle & WS_EX_TOOLWINDOW) && strcmp(className, "ApplicationFrameWindow") != 0) {
            return TRUE;
        }
        ShowWindowAsync(hwnd, SW_FORCEMINIMIZE);
    }
    return TRUE;
}

void TriggerShowDesktop() {
    INPUT inputs[4] = {0};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'M';
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'M';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_LWIN;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));

    Sleep(30);
    EnumWindows(EnumWindowsProc, 0);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TRAYICON:
            switch (lParam) {
                case WM_LBUTTONUP:
                    TriggerShowDesktop();
                    break;
                case WM_RBUTTONUP: {
                    POINT pt;
                    GetCursorPos(&pt);
                    HMENU hMenu = CreatePopupMenu();
                    AppendMenuA(hMenu, MF_STRING, IDM_INSTALL, "Install to System");
                    AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
                    AppendMenuA(hMenu, MF_STRING, IDM_ADD_STARTUP, "Add current to Startup");
                    AppendMenuA(hMenu, MF_STRING, IDM_REM_STARTUP, "Remove from Startup");
                    AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
                    AppendMenuA(hMenu, MF_STRING, IDM_EXIT, "Exit");
                    SetForegroundWindow(hwnd);
                    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                    DestroyMenu(hMenu);
                    break;
                }
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_INSTALL:
                    InstallToProgramFiles();
                    break;
                case IDM_ADD_STARTUP:
                    AddToStartup();
                    break;
                case IDM_REM_STARTUP:
                    RemoveFromStartup();
                    break;
                case IDM_EXIT:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void RunApplication(HINSTANCE hInstance) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "DesktopHelper";
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, "", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
    strcpy(nid.szTip, "Minimize All Windows");
    Shell_NotifyIcon(NIM_ADD, &nid);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);

    Shell_NotifyIcon(NIM_DELETE, &nid);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
    if (!IsUserAnAdmin()) {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        SHELLEXECUTEINFOA sei = { sizeof(sei) };
        sei.lpVerb = "runas";
        sei.lpFile = path;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;
        if (ShellExecuteExA(&sei)) {
            return 0;
        }
    }

    RunApplication(hInstance);
    return 0;
}
