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

implant:
	$(CXX) $(CFLAGS) -DDEBUG=1 $(SRCS) -o bin/implant


