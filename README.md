Backstory, startallback x64 for windows 11's showdesktop feature is broken so I made this small utility.

### **AI DISCLOSURE:** `The following readme was generated via a locally hosted gemma e4b model:`

# Desktop Helper

A lightweight Win32 system tray application designed to instantly minimize all visible windows via native keystroke injection and window enumeration. The project separates the application shell from its persistence and deployment routines, which are compiled into a distinct static library.

## Features

* **Instant Minimize:** Left-click the system tray icon to clean your desktop.
* **System Integration:** Right-click menu option to copy the executable to `Program Files\DesktopHelper` and generate a Start Menu shortcut.
* **Persistence Management:** Toggle application launch at Windows startup via the current user registry run keys.
* **UAC Elevation:** Automatically attempts to relaunch with administrator privileges if required for system installation.

## Project Structure

```text
├── dist/                  # Production binaries
├── objs/                  # Intermediate object files, .lib, and .res
├── installer.c / .h       # Installation and registry persistence logic
├── main.c                 # WinMain, WindowProc, and Tray UI loop
├── res.rc                 # Application resource script (Icons/Context menus)
└── makefile               # Clang + windres automation build file

```

## Prerequisites

The build system expects an LLVM/Clang toolchain and GNU Make matching a Windows target environment (such as LLVM paired with MSYS2/MinGW tools):

* `clang` (Compiler)
* `llvm-lib` (Static Librarian)
* `windres` (Windows Resource Compiler)
* `make` (Build automation tool)

## Compilation

Build the release executable and static libraries:

```bash
make

```

To wipe intermediate object states and clear the build artifacts:

```bash
make clean

```

## Architecture Notes

* **Keystroke Simulation:** Emulates `Win + M` via `SendInput` to leverage the OS layout dispatcher, followed by an explicit `EnumWindows` cycle using `ShowWindowAsync(hwnd, SW_FORCEMINIMIZE)` to force minimization on windows that reject the initial shell command.
* **Modularity:** The deployment system (`installer.c`) is strictly separated from the runtime logic (`main.c`). It handles registry modifications (`Software\Microsoft\Windows\CurrentVersion\Run`) and COM interface initialization (`IShellLinkA`/`IPersistFile`) safely outside the application engine loop.



-------------

`app.ico` <a href="https://www.flaticon.com/free-icons/monitor" title="monitor icons">Hilmy Abiyyu A. - Flaticon</a>
