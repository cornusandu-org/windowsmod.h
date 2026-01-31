# for linux (binaries will run on windows). you can adapt for windows if you want.
CC=x86_64-w64-mingw32-g++
CFLAGS= -O1 -Wall -fstack-protector-strong -fno-delete-null-pointer-checks -Wtrampolines -Wshadow -Werror=return-type -Werror=unused-function -Wno-write-strings

.PHONY: all, clean, test, delete

all: windowsmod.dll
	@echo "+ Build complete!"

windowsmod.dll:	./file.o ./reg.o ./err.o ./init.o ./Makefile
# @echo "/ Building .o -> windowsmod.dll"
	@$(CC) $(CFLAGS) ./file.o ./reg.o ./err.o -shared -Wl,--out-implib,libwindowsmod.dll.a -o ./windowsmod.dll
	@echo "+ Built .o -> windowsmod.dll"

file.o: ./src/windowsmod_file.cpp ./include/windowsmod.hpp ./Makefile
# @echo "/ Building windowsmod_file.cpp -> file.o"
	@$(CC) $(CFLAGS) ./src/windowsmod_file.cpp -c -o ./file.o
	@echo "+ Built windowsmod_file.cpp -> file.o"

reg.o: ./src/windowsmod_reg.cpp ./include/windowsmod.hpp ./Makefile
# @echo "/ Building windowsmod_reg.cpp -> reg.o"
	@$(CC) $(CFLAGS) ./src/windowsmod_reg.cpp -c -o ./reg.o
	@echo "+ Built windowsmod_reg.cpp -> reg.o"

err.o: ./src/error.cpp ./include/windowsmod.hpp ./Makefile
# @echo "/ Building error.cpp -> err.o"
	@$(CC) $(CFLAGS) ./src/error.cpp -c -o ./err.o
	@echo "+ Built error.cpp -> err.o"

init.o: ./src/init.cpp ./include/windowsmod.hpp ./Makefile
# @echo "/ Building init.cpp -> init.o"
	@$(CC) $(CFLAGS) ./src/init.cpp -c -o ./init.o
 
test: myprogram.exe

myprogram.exe: windowsmod.dll ./Makefile example.cpp
	x86_64-w64-mingw32-g++ example.cpp   -L./   -lwindowsmod   -o myprogram.exe

clean:
	rm -f *.o

delete: clean
	rm -f *.dll *.a *.exe
