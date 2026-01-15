#include "../include/windowsmod.hpp"
#include "error.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct hkeyCloseDataAtExit {
    HKEY key;
    char toClose;
    char inUse;
};



struct hkeyCloseDataAtExit *hkeyCloseDataAtExitList = NULL;
size_t hkeyCloseDataAtExitListSize = 0;
size_t hkeyCloseDataAtExitListIndex = 0;

void windowsmod_init() {
    hkeyCloseDataAtExitList = (hkeyCloseDataAtExit*)malloc(4 * sizeof(struct hkeyCloseDataAtExit));
    hkeyCloseDataAtExitListSize = 4;
}

void hkeyCloseDataAtExitList_append(HKEY key, char inUse) {
    if (hkeyCloseDataAtExitListIndex < hkeyCloseDataAtExitListSize) {
        hkeyCloseDataAtExitList[hkeyCloseDataAtExitListIndex] = (struct hkeyCloseDataAtExit){.key = key, .toClose = 1, .inUse = inUse};
        hkeyCloseDataAtExitListIndex++;
    } else {
        size_t old_size = hkeyCloseDataAtExitListSize;
        hkeyCloseDataAtExitListSize += 4;
        struct hkeyCloseDataAtExit *newHkeyCloseDataAtExitList = (hkeyCloseDataAtExit*)malloc(hkeyCloseDataAtExitListSize * sizeof(struct hkeyCloseDataAtExit));
        memcpy(newHkeyCloseDataAtExitList, hkeyCloseDataAtExitList, old_size * sizeof(struct hkeyCloseDataAtExit));
        free(hkeyCloseDataAtExitList);
        hkeyCloseDataAtExitList = newHkeyCloseDataAtExitList;
        hkeyCloseDataAtExitList[hkeyCloseDataAtExitListIndex] = (struct hkeyCloseDataAtExit){.key = key, .toClose = 1, .inUse = inUse};
        hkeyCloseDataAtExitListIndex++;
    }
}

HKEY *queryRegistry(const char* query, HKEY *parent) {
    HKEY *hKey = (HKEY*)malloc(sizeof(HKEY));
    if (!hKey) return 0x0;
    if (RegOpenKeyExA(parent != NULL ? *parent : HKEY_CURRENT_USER,
                      query,
                      0, KEY_READ, hKey) != ERROR_SUCCESS) {
        free(hKey);
        return 0;
    };

    hkeyCloseDataAtExitList_append(*hKey, 1);

    return hKey;
}

struct readValueFromRegistryOutput readValueFromRegistry(HKEY hKey, const char* query) {
    DWORD dwType;
    DWORD cbData = 0;

    LONG result = RegQueryValueExA(hKey, query, NULL, &dwType, NULL, &cbData);

    if (result != ERROR_SUCCESS) {
        lastError = "[REGISTRY+R] readValueFromRegistry: (result!=ERROR_SUCCESS) Failed to retrieve info";
        return (struct readValueFromRegistryOutput){.data = 0, .err=1};  // Failed to retrieve info
    }

    if (cbData > 4096 * 4) {
        lastError = "[REGISTRY+R] readValueFromRegistry: (cbData>4096*4) Attempted to read more than 16KB of data from a single registry entry";
        return (struct readValueFromRegistryOutput){.data = 0, .err=4};  // Your shit code needs registry entries worth more than 16KB of data per entry
    }

    if (dwType == REG_SZ) {
        char buffer[4096*4+1];
        // 3. Query the value again to get the data
        result = RegQueryValueExA(hKey, query, NULL, &dwType, (LPBYTE)buffer, &cbData);
        if (result == ERROR_SUCCESS) {
            char* data = (char*)malloc(cbData+1);
            memcpy(data, buffer, cbData);
            return (struct readValueFromRegistryOutput){.data = data, .err=0};
        } else {
            lastError = "[REGISTRY+R] readValueFromRegistry: (result!=ERROR_SUCCESS) Failed to read value from registry entry";
            return (struct readValueFromRegistryOutput){.data = 0, .err=2};  // Failed to read
        }
    }
    else if (dwType == REG_DWORD) {
        DWORD _data;
        result = RegQueryValueExA(hKey, query, NULL, &dwType, (LPBYTE)&_data, &cbData);
        if (result == ERROR_SUCCESS) {
            char buffer[4096*4];
            sprintf(buffer, "%llu", (size_t)_data);
            char* data = (char*)malloc(strlen(buffer)+1);
            memcpy(data, buffer, strlen(buffer) + 1);
            return (struct readValueFromRegistryOutput){.data = data, .err=0};
        } else {
            lastError = "[REGISTRY+R] readValueFromRegistry: (result!=ERROR_SUCCESS) Failed to read value from registry entry";
            return (struct readValueFromRegistryOutput){.data = 0, .err=2};  // Failed to read
        }
    }
    else if (dwType == REG_QWORD) {
        long unsigned int _data;
        result = RegQueryValueExA(hKey, query, NULL, &dwType, (LPBYTE)&_data, &cbData);
        if (result == ERROR_SUCCESS) {
            char buffer[4096*4];
            sprintf(buffer, "%llu", (size_t)_data);
            char* data = (char*)malloc(strlen(buffer)+1);
            memcpy(data, buffer, strlen(buffer) + 1);
            return (struct readValueFromRegistryOutput){.data = data, .err=0};
        } else {
            lastError = "[REGISTRY+R] readValueFromRegistry: (result!=ERROR_SUCCESS) Failed to read value from registry entry";
            return (struct readValueFromRegistryOutput){.data = 0, .err=2};  // Failed to read
        }
    }
    else{
        lastError = "[REGISTRY+R] readValueFromRegistry: (dwType!=...) Value is not of a recognized type";
        return (struct readValueFromRegistryOutput){.data = 0, .err=3};  // Value is not of a recognized type
    }
}

char existsInRegistry(const char* query, HKEY *parent) {
    HKEY hKey;
    LONG result = RegOpenKeyExA(
        parent != NULL ? *parent : HKEY_CURRENT_USER,
        query,
        0,
        KEY_READ,
        &hKey
    );

    if (result == ERROR_SUCCESS) {
        hkeyCloseDataAtExitList_append(hKey, 0);
        return 1;
    }

    if (result == ERROR_FILE_NOT_FOUND) {
        return 0;
    }

    return 0;
}

HKEY *createRegistryEntry(const char* query, HKEY *parent) {
    HKEY *outKey = (HKEY*)malloc(sizeof(HKEY));

    LONG result = RegCreateKeyExA(
        parent ? *parent : HKEY_CURRENT_USER,                  // defaults to HKEY_CURRENT_USER
        query,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        outKey,
        NULL
    );

    if (result != ERROR_SUCCESS || outKey == NULL) {
        free(outKey);
        lastError = "[REGISTRY+W] createRegistryEntry: (result!=ERROR_SUCCESS||outKey==NULL) Failed to create registry entry";
        return 0x0;
    }

    hkeyCloseDataAtExitList_append(*outKey, 1);

    return outKey;
}

size_t set_string_value(const char* const query, const char* const value, HKEY *parent) {
    return RegSetValueExA(
        parent ? *parent : HKEY_CURRENT_USER,
        query,
        0,
        REG_SZ,
        (const BYTE * const)value,
        (DWORD)(strlen(value) + 1)  // include null terminator
    );
}

size_t set_64int_value(const char* const query, const int64_t value, HKEY *parent) {
    return RegSetValueExA(
        parent ? *parent : HKEY_CURRENT_USER,
        query,
        0,
        REG_QWORD,
        (const BYTE * const)&value,
        sizeof(value)
    );
}

size_t set_32int_value(const char* const query, const DWORD value, HKEY *parent) {
    return RegSetValueExA(
        parent ? *parent : HKEY_CURRENT_USER,
        query,
        0,
        REG_DWORD,
        (const BYTE * const)&value,
        sizeof(value)
    );
}

void closeHandle(HKEY *handle) {
    for (struct hkeyCloseDataAtExit *p = hkeyCloseDataAtExitList; p < hkeyCloseDataAtExitList + hkeyCloseDataAtExitListIndex; p++) {
        if (p->key == *handle) {
            p->inUse = 0;
            p->toClose = 0;
            RegCloseKey(p->key);
            return;
        }
    }
}

void cleanupHandles() {
    size_t freedSize = 0;
    for (struct hkeyCloseDataAtExit *p = hkeyCloseDataAtExitList; p < hkeyCloseDataAtExitList + hkeyCloseDataAtExitListIndex; p++) {
        if (!p->toClose) {
            freedSize++;
        } else if (!p->inUse) {
            if (p->toClose) {
                p->toClose = 0;
                RegCloseKey(p->key);
            }
            freedSize++;
        }
    }
    size_t index = 0;

    struct hkeyCloseDataAtExit *newlist = (hkeyCloseDataAtExit*)malloc(sizeof(struct hkeyCloseDataAtExit) * ((hkeyCloseDataAtExitListIndex - freedSize) ? (hkeyCloseDataAtExitListIndex - freedSize) : 4));
    if (!newlist) {
        return;
    }

    for (struct hkeyCloseDataAtExit *p = hkeyCloseDataAtExitList; p < hkeyCloseDataAtExitList + hkeyCloseDataAtExitListIndex; p++) {
        if (p->toClose) {
            memcpy(newlist + index, p, sizeof(struct hkeyCloseDataAtExit));
            index++;
        }
    }

    free(hkeyCloseDataAtExitList);
    hkeyCloseDataAtExitList = newlist;
    hkeyCloseDataAtExitListSize = index ? index : 4;
    hkeyCloseDataAtExitListIndex = index;
}
 