# List of source files
FILES = ./build/parser/symboltable/symboltable.o ./build/parser/commands/commands_new.o  ./build/parser/parser.o ./build/utils/common/customstring.o ./build/utils/heap/h_usage.o ./build/utils/memory/memory.o ./build/c_client.o ./build/p_poll.o ./build/string.o ./build/p_server.o ./build/pie.o
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
# Default target
all : pie

# Compile the final executable
pie : $(FILES)	
	gcc -std=c99 $(FILES) -o ./bin/pie.bin

./build/parser/symboltable/symboltable.o : ./src/parser/symboltable/symboltable.c
	gcc -c -std=c99 ./src/parser/symboltable/symboltable.c -o ./build/parser/symboltable/symboltable.o

./build/parser/commands/commands_new.o : ./src/parser/commands/commands_new.c
	gcc -c -std=c99 ./src/parser/commands/commands_new.c -o ./build/parser/commands/commands_new.o

./build/parser/commands/commands.o : ./src/parser/commands/commands.c
	gcc -c -std=c99 ./src/parser/commands/commands.c -o ./build/parser/commands/commands.o

./build/parser/parser.o : ./src/parser/parser.c
	gcc -c -std=c99 ./src/parser/parser.c -o ./build/parser/parser.o

./build/utils/common/customstring.o : ./src/utils/common/customstring.c
	gcc -c -std=c99 ./src/utils/common/customstring.c -o ./build/utils/common/customstring.o

# Compile h_usage.o
./build/utils/heap/h_usage.o : ./src/utils/heap_usage/h_usage.c
	gcc -c -std=c99 ./src/utils/heap_usage/h_usage.c -o ./build/utils/heap/h_usage.o

# Compile h_usage.o
./build/utils/memory/memory.o : ./src/utils/memory/p_memory.c
	gcc -c -std=c99 ./src/utils/memory/p_memory.c -o ./build/utils/memory/memory.o

# Compile client.o
./build/c_client.o : ./src/connections/clients.c
	gcc -c -std=c99 ./src/connections/clients.c -o ./build/c_client.o

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
	rm -rf ${FILES}
	cp -r ./bin ./binback
	rm -rf ./bin/*

# Phony targets
.PHONY: all clean
