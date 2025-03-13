# Compiler and Tools
ARCH := aarch64-linux-gnu
CC := $(ARCH)-gcc
CXX := $(ARCH)-g++
LD := $(ARCH)-ld

# Directories
SRC_DIR := ./src
INC_DIR := ./inc
BUILD_DIR := ./build
BIN_DIR := ./bin

# Source files and corresponding object and binary files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
BINS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRCS))

# Compiler and linker flags
CFLAGS := -Wall -O2 -I$(INC_DIR)
LDFLAGS :=

# Default target: build all binaries
.PHONY: all clean

all: implant

# Create necessary directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

example_http: 
	$(CC) $(CFLAGS) $(BUILD_DIR)/http_client.o $(SRC_DIR)/example_http.c -o $(BIN_DIR)/example_http

example_file_iter: 
	$(CC) $(CFLAGS) $(BUILD_DIR)/file_iter.o $(SRC_DIR)/example_file_iter.c -o $(BIN_DIR)/example_file_iter


example_exec_stream: 
	$(CC) $(CFLAGS) $(BUILD_DIR)/exec_stream.o $(SRC_DIR)/example_exec_stream.c -o $(BIN_DIR)/example_exec_stream

libspeedrun: example_exec_stream example_file_iter example_http
	echo "Built speedrun"


implant:
	$(CXX) $(CFLAGS) -DDEBUG=1 $(SRCS) -o bin/implant

sanatize_implant:
	aarch64-linux-gnu-g++ -Iinc -DSLEEP_TIME=2 -DDEBUG -fsanitize=address -g  build/* src/json
_http.cpp src/implant.cpp -o bin/implant

