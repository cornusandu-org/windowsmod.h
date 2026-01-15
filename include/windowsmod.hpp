#pragma once
#include <stdint.h>
#include <windows.h>
#include <memory>

namespace win32 { namespace fs {
    using FileCreatePerms = size_t;
    using FileAttributes = size_t;
}}
namespace win32 { namespace fs {
    static const constexpr DWORD _FILE_EXECUTE = FILE_EXECUTE;
    static const constexpr DWORD _FILE_SHARE_READ = FILE_SHARE_READ;
    static const constexpr DWORD _FILE_SHARE_WRITE = FILE_SHARE_WRITE;
    static const constexpr DWORD _FILEEXISTS_CREATE_NEW = CREATE_NEW;
    static const constexpr DWORD _FILEEXISTS_CREATE_ALWAYS = CREATE_ALWAYS;
    static const constexpr DWORD _FILEEXISTS_OPEN_EXISTING = OPEN_EXISTING;
    static const constexpr DWORD _FILEEXISTS_OPEN_ALWAYS = OPEN_ALWAYS;
}}
#undef FILE_EXECUTE
#undef FILE_SHARE_READ
#undef FILE_SHARE_WRITE
#undef CREATE_NEW
#undef CREATE_ALWAYS
#undef OPEN_EXISTING
#undef OPEN_ALWAYS
#undef TRUNCATE_EXISTING

namespace enum_ {
    namespace fs {
        namespace FileCreatePerms {
            enum FileCreatePermsEnum : size_t {
                FILE_READ = 1 << 1,
                FILE_WRITE = 1 << 2,
                FILE_EXECUTE = 1 << 3,
                FILE_SHARE_READ = 1 << 4,
                FILE_SHARE_WRITE = 1 << 5
            };
        }
        namespace FileAttributes {
            enum FileAttributesEnum : size_t {
                NORMAL = FILE_ATTRIBUTE_NORMAL,
                HIDDEN = FILE_ATTRIBUTE_HIDDEN,
                SYSTEM_FILE = FILE_ATTRIBUTE_SYSTEM,
                TEMPORARY = FILE_ATTRIBUTE_TEMPORARY,
                READONLY_FILE = FILE_ATTRIBUTE_READONLY,
                TOARCHIVE = FILE_ATTRIBUTE_ARCHIVE,
                DELETE_ON_CLOSE = FILE_FLAG_DELETE_ON_CLOSE,
                WRITE_TO_DISK = FILE_FLAG_WRITE_THROUGH

                #undef FILE_ATTRIBUTE_NORMAL
                #undef FILE_ATTRIBUTE_HIDDEN
                #undef FILE_ATTRIBUTE_SYSTEM
                #undef FILE_ATTRIBUTE_TEMPORARY
                #undef FILE_ATTRIBUTE_READONLY
                #undef FILE_ATTRIBUTE_ARCHIVE
                #undef FILE_FLAG_DELETE_ON_CLOSE
                #undef FILE_FLAG_WRITE_THROUGH
            };
        }
    }
}

namespace win32 { namespace fs {
    static const constexpr DWORD FILE_EXECUTE = _FILE_EXECUTE;
    static const constexpr DWORD FILE_SHARE_READ = _FILE_SHARE_READ;
    static const constexpr DWORD FILE_SHARE_WRITE = _FILE_SHARE_WRITE;
    static const constexpr DWORD CREATE_NEW = _FILEEXISTS_CREATE_NEW;
    static const constexpr DWORD CREATE_ALWAYS = _FILEEXISTS_CREATE_ALWAYS;
    static const constexpr DWORD OPEN_EXISTING = _FILEEXISTS_OPEN_EXISTING;
    static const constexpr DWORD OPEN_ALWAYS = _FILEEXISTS_OPEN_ALWAYS;
}}

void windowsmod_init();  // MUST BE CALLED AT THE START OF THE PROGRAM

struct readValueFromRegistryOutput {char* data; size_t err;};

HKEY* queryRegistry(const char* query, HKEY *parent);
struct readValueFromRegistryOutput readValueFromRegistry(HKEY handle, const char* query);
char existsInRegistry(const char* query, HKEY *parent);

HKEY *createRegistryEntry(const char* query, HKEY *parent);
size_t set_string_value(const char* const query, const char* const value, HKEY *parent);
size_t set_64int_value(const char* const query, const int64_t value, HKEY *parent);
size_t set_32int_value(const char* const query, const DWORD value, HKEY *parent);

extern "C" const char* const getLastError();

void closeHandle(HKEY *handle);
void cleanupHandles();

std::shared_ptr<void> createFile(const char* path, win32::fs::FileCreatePerms perms, win32::fs::FileAttributes attributes, DWORD if_already_exists = win32::fs::OPEN_ALWAYS);
size_t getFileSize(const char* path);
size_t getFileSizeFromHandle(std::shared_ptr<void> handle);
char fileExists(const char* path);
