# for linux (binaries will run on windows). you can adapt for windows if you want.
CC=x86_64-w64-mingw32-gcc
CFLAGS= -O1 -Wall -fstack-protector-strong -fno-delete-null-pointer-checks -Wtrampolines -Wshadow -Werror=return-type -Werror=unused-function

.PHONY: all, clean

all: windowsmod.dll
	@echo "+ Build complete!"

windowsmod.dll:	./src/windowsmod.c ./include/windowsmod.h
	@echo "/ Building windowsmod.c -> windowsmod.dll"
	@$(CC) $(CFLAGS) ./src/windowsmod.c -shared -o ./windowsmod.dll
	@echo "+ Built windowsmod.c -> windowsmod.dll"