#include "include/windowsmod.hpp"

int main() {
    windowsmod_init();

    createFile("./test.txt", enum_::fs::FileCreatePerms::FILE_READ, enum_::fs::FileAttributes::READONLY_FILE, 0);

    return 0;
}
