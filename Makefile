#Toolchain
CC := gcc
CXX := g++

LD := $(CXX)

# Compiler and Linker flags
TLD					:= ./
INCLUDE_DIRS		:= $(TLD)/ ./src
LINKER_INPUTS		:= -lrt

CXXFLAGS := -std=c++17 -fdata-sections -ffunction-sections -O0  -Wall -Wno-format -Wno-unused-but-set-variable -Wno-unused-variable -Wno-unused-local-typedefs
CXXFLAGS += $(addprefix -I,$(INCLUDE_DIRS))
VPATH=src/

# Source file
SOURCES = src/app_main.cpp 
# Object file
OBJ = $(SOURCES:.cpp=.o)
# Output binary name
PROGRAM = json_serialize

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
		$(CXX) $(CXXFLAGS) -o ./bin/$(PROGRAM) $(OBJ) $(LINKER_INPUTS)

.SUFFIXES = .cpp

.cpp.o:
		$(CXX) $(INCLUDE) $(CXXFLAGS) -c $< -o $@

clean:
		rm  ./src/*.o \
			./bin/$(PROGRAM)
