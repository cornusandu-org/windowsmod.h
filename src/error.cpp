#include "error.hpp"
#include "../include/windowsmod.hpp"

char* lastError = "";

const char* const getLastError() {
    return lastError;
}
 