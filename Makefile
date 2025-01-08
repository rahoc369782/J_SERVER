# ifdef CC
# CC := $(shell which $(CC))
# else
# CC := $(shell which gcc)
# endif

# ifeq ($(findstring clang, $(CC)), clang)
FLAGS = -g -ffreestanding -fomit-frame-pointer -finline-functions -Wno-unused-function \
        -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
        -nostdlib -nodefaultlibs -Wall -O0 -Iinc
# else
# FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops \
#         -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function \
#         -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter \
#         -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
# endif

# List of source files
FILES = ./build/parser/grammer/grammer.o ./build/parser/tokenization/tokenization.o \
        ./build/parser/parser.o ./build/utils/common/customstring.o \
        ./build/utils/heap/h_usage.o ./build/utils/memory/memory.o \
        ./build/c_client.o ./build/p_poll.o ./build/string.o \
        ./build/p_server.o ./build/pie.o

# Default target
all: pie

# Compile the final executable
pie: $(FILES)
	gcc -std=c99 $(FILES) -o ./bin/pie.bin

# Object file rules with directory creation
./build/parser/grammer/grammer.o: ./src/parser/grammer/grammer.c
	mkdir -p $(dir $@)
	gcc -c -std=c99 $< -o $@

./build/parser/symboltable/symboltable.o: ./src/parser/symboltable/symboltable.c
	mkdir -p $(dir $@)
	gcc -c -std=c99 $< -o $@

./build/parser/tokenization/tokenization.o: ./src/parser/tokenization/tokenization.c
	mkdir -p $(dir $@)
	gcc -c -std=c99 $< -o $@

./build/parser/tokenization/commands.o: ./src/parser/tokenization/commands.c
	mkdir -p $(dir $@)
	gcc -c -std=c99 $< -o $@

./build/parser/parser.o: ./src/parser/parser.c
	mkdir -p $(dir $@)
	gcc -c -std=c99 $< -o $@

./build/utils/common/customstring.o: ./src/utils/common/customstring.c
	mkdir -p $(dir $@)
	gcc -c -std=c99 $< -o $@

./build/utils/heap/h_usage.o: ./src/utils/heap_usage/h_usage.c
	mkdir -p $(dir $@)
	gcc -c -std=c99 $< -o $@

./build/utils/memory/memory.o: ./src/utils/memory/p_memory.c
	mkdir -p $(dir $@)
	gcc -c -std=c99 $< -o $@

./build/c_client.o: ./src/connections/clients.c
	mkdir -p $(dir $@)
	gcc -c -std=c99 $< -o $@

./build/p_poll.o: ./src/p_poll/p_poll.c
	mkdir -p $(dir $@)
	gcc -c $(FLAGS) -std=c99 $< -o $@

./build/string.o: ./src/server/string/string.c
	mkdir -p $(dir $@)
	gcc -c $(FLAGS) -std=c99 $< -o $@

./build/p_server.o: ./src/server/syscalls/p_server.c
	mkdir -p $(dir $@)
	gcc -c $(FLAGS) -std=c99 $< -o $@

./build/pie.o: ./src/pie.c
	mkdir -p $(dir $@)
	gcc -c $(FLAGS) -std=c99 $< -o $@

# Clean build directory
clean:
	rm -rf $(FILES)
	cp -r ./bin ./binback
	rm -rf ./bin/*

# Phony targets
.PHONY: all clean