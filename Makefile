#Makefile for the interpreter, bsi

CXX := clang++

SRC_DIR := src/interpreter
OBJ_DIR := obj/interpreter
INC_DIR := src/include/interpreter

BUILD_TYPE ?= DEBUG
FLAGS = -std=c++17 -Wall -I$(INC_DIR)

ifeq ($(BUILD_TYPE), DEBUG) 
	FLAGS += -g
else 
	ifeq ($(BUILD_TYPE), RELEASE)
		FLAGS += -O2
	endif
endif

_DEPS = Brainf.hpp Memory.hpp Program.hpp
DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

_OBJ = main.o Brainf.o Memory.o Program.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

$(shell mkdir -p obj/interpreter)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) -c $(FLAGS) -o $@ $<

bsi: $(OBJ)
	$(CXX) $(FLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -rfd obj bsi
