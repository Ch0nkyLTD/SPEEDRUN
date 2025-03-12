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
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
BINS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRCS))

EXAMPLE := $(wildcard $(SRC_DIR)/example_*.c)
# Compiler and linker flags
CFLAGS := -Wall -O2 -I$(INC_DIR)
LDFLAGS :=

# Default target: build all binaries
.PHONY: all clean

all: $(BINS)


# Create necessary directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

http_client.o:
	$(CC) $(CFLAGS) -c  $(SRC_DIR)/http_client.c -o $(BUILD_DIR)/http_client.o

example_http:http_client.o 
	$(CC) $(CFLAGS) $(BUILD_DIR)/http_client.o $(SRC_DIR)/example_http.c -o $(BIN_DIR)/example_http


file_iter.o:
	$(CC) $(CFLAGS) -c  $(SRC_DIR)/file_iter.c -o $(BUILD_DIR)/file_iter.o

example_file_iter: file_iter.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/file_iter.o $(SRC_DIR)/example_file_iter.c -o $(BIN_DIR)/example_file_iter


exec_stream.o:
	$(CC) $(CFLAGS) -c  $(SRC_DIR)/exec_stream.c -o $(BUILD_DIR)/exec_stream.o


example_exec_stream: exec_stream.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/exec_stream.o $(SRC_DIR)/example_exec_stream.c -o $(BIN_DIR)/example_exec_stream

libspeedrun: example_exec_stream example_file_iter example_http
	echo "Built speedrun"

student_bundle:
	tar czfv bin/student_bundle.tar.gz inc/ build/ $(EXAMPLE) Makefile


# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

