CXX = clang++
BUILD_TYPE = DEBUG
FLAGS = -Wall

ifeq ($(BUILD_TYPE), DEBUG) 
	FLAGS = -g $(FLAGS)
else 
	ifeq ($(BUILD_TYPE), RELEASE)
		FLAGS = -O2 $(FLAGS)
endif

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = src/include

_DEPS = Interpreter.hpp Memory.hpp Program.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = Memory.o Program.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c $(FLAGS) -o $@ $<

bsi: $(OBJ)
	$(OBJ) $(FLAGS) -o $@ $^

clean:
	rm -f $(ODIR)/*.o bsi