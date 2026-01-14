#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void windowsmod_init();  // MUST BE CALLED AT THE START OF THE PROGRAM

struct readValueFromRegistryOutput {char* data; size_t err;};

HKEY* queryRegistry(const char* query, HKEY *parent);
struct readValueFromRegistryOutput readValueFromRegistry(HKEY handle, const char* query);
char existsInRegistry(const char* query, HKEY *parent);

HKEY *createRegistryEntry(const char* query, HKEY *parent);
size_t set_string_value(const char* const query, const char* const value, HKEY *parent);
size_t set_64int_value(const char* const query, const int64_t value, HKEY *parent);
size_t set_32int_value(const char* const query, const DWORD value, HKEY *parent);

const char* const getLastError();

void closeHandle(HKEY *handle);
void cleanupHandles();

#ifdef __cplusplus
}
#endif
