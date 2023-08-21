# List of source files
FILES = ./build/p_poll.o ./build/string.o ./build/p_server.o ./build/pie.o
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
# Default target
all : pie

# Compile the final executable
pie : $(FILES)
	echo "Compiling main pie binary"
	gcc -std=c99 $(FILES) -o ./bin/pie

# Compile epoll.o
./build/p_poll.o : ./src/p_poll/p_poll.c
	gcc -c $(FLAGS) -std=c99 ./src/p_poll/p_poll.c -o ./build/p_poll.o

# Compile p_server.o
./build/string.o : ./src/server/string/string.c
	gcc -c $(FLAGS) -std=c99 ./src/server/string/string.c -o ./build/string.o

# Compile p_server.o
./build/p_server.o : ./src/server/syscalls/p_server.c
	gcc -c $(FLAGS) -std=c99 ./src/server/syscalls/p_server.c -o ./build/p_server.o

# Compile pie.o
./build/pie.o : ./src/pie.c
	gcc -c $(FLAGS) -std=c99 ./src/pie.c -o ./build/pie.o

# Clean build directory
clean : 
	rm -rf ./build/*
	cp -r ./bin ./binback
	rm -rf ./bin/*

# Phony targets
.PHONY: all clean
