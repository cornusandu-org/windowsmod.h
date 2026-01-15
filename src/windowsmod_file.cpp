#include "../include/windowsmod.hpp"
#include "error.hpp"

#include <string>
#include <cstring>
#include <cstdlib>

using namespace enum_::fs;

DWORD mapFilePermToWindowsPerm(size_t perms) {
    DWORD p = 0;
    if (perms & FileCreatePerms::FILE_READ) p |= GENERIC_READ;
    if (perms & FileCreatePerms::FILE_WRITE) p |= GENERIC_WRITE;
    if (perms & FileCreatePerms::FILE_EXECUTE) p |= GENERIC_EXECUTE;
    return p;
}

DWORD mapSharedFilePermToWindowsPerm(size_t perms) {
    DWORD p = 0;
    if (perms & FileCreatePerms::FILE_SHARE_READ) p |= win32::fs::FILE_SHARE_READ;
    if (perms & FileCreatePerms::FILE_SHARE_WRITE) p |= win32::fs::FILE_SHARE_WRITE;
    return p;
}

void* createFile(const char* path, win32::fs::FileCreatePerms perms, win32::fs::FileAttributes attributes, DWORD if_already_exists) {
    if (!path) {
        lastError = "[FS:W] createFile: (!path) Recieved null pointer to path";
        return 0x0;
    }
    HANDLE handle = CreateFileA(path, mapFilePermToWindowsPerm(perms), mapSharedFilePermToWindowsPerm(perms), NULL, if_already_exists, attributes, NULL);
    if (handle == INVALID_HANDLE_VALUE || !handle) {
        char msg[4096]{0};
        sprintf(msg, "[FS:W] createFile: (!handle||...) CreateFileA() returned an invalid handle (#%llu)", (size_t)GetLastError());
        lastError = msg;
    }

    return handle;
}
 