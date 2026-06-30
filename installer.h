#ifndef INSTALLER_H
#define INSTALLER_H

#include <windows.h>

void AddToStartup(const char* appName);
void RemoveFromStartup(const char* appName);
void InstallToProgramFiles(const char* appName, const char* subDir, const char* exeName, const char* shortcutName, const char* shortcutDesc);

#endif
