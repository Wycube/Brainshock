# Makefile for the interpreter, bsi

CXX := clang++

SRC_DIR := src
OBJ_DIR := build/obj
INC_DIR := include
LIB_DIR := lib/lest

BUILD_TYPE ?= DEBUG
FLAGS = -std=c++17 -Wall -I$(INC_DIR) -I$(LIB_DIR)

ifeq ($(BUILD_TYPE), DEBUG) 
	FLAGS += -g
else 
	ifeq ($(BUILD_TYPE), RELEASE)
		FLAGS += -O2
	endif
endif

_DEPS = Interpreter.hpp Memory.hpp Program.hpp
DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

_OBJ = main.o Interpreter.o Memory.o Program.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

$(shell mkdir -p build/obj)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) -c $(FLAGS) -o $@ $<

build/bsi: $(OBJ)
	$(CXX) $(FLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rfd build
