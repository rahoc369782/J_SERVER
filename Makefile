# List of source files
FILES = ./build/pie.o ./build/p_server.o

# Default target
all : pie

# Compile the final executable
pie : $(FILES)
	gcc -std=c99 $(FILES) -o ./bin/pie

# Compile p_server.o
./build/p_server.o : ./src/server/p_server.c
	gcc -c -std=c99 ./src/server/p_server.c -o ./build/p_server.o

# Compile pie.o
./build/pie.o : ./src/server/pie.c
	gcc -c -std=c99 ./src/server/pie.c -o ./build/pie.o

# Clean build directory
clean : 
	rm -rf ./build/*

# Phony targets
.PHONY: all clean
