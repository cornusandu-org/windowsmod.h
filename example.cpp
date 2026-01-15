#include "include/windowsmod.hpp"
#include <cstdio>

int main() {
    windowsmod_init();

    std::shared_ptr<void> h0 = createFile("test.txt", enum_::fs::FileCreatePerms::FILE_READ, enum_::fs::FileAttributes::READONLY_FILE);

    std::shared_ptr<void> h1 = createFile(0x0, enum_::fs::FileCreatePerms::FILE_READ, enum_::fs::FileAttributes::READONLY_FILE);

    if (!h1.get()) {
        printf(getLastError());
    }

    return 0;

}
