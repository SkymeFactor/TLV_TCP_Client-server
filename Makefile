#------------------------------------------------------------
#	Variables
#------------------------------------------------------------

# Compiler
CXX=g++
CXXFLAGS=-std=c++17 -lpthread -pthread

# Directories
SRC_DIR=src/
OBJ_DIR=obj/
INCLUDE_DIR=$(SRC_DIR)include/
BIN_DIR=build/

# Files
SOURCES=$(wildcard $(SRC_DIR)*.cpp)
OBJECTS=$(SOURCES:$(SRC_DIR)%.cpp=%.o)
INCLUDE=-I $(INCLUDE_DIR)
BINARIES=server.elf client.elf

# Cleaner
RM=rm -rf

#------------------------------------------------------------
#	Targets
#------------------------------------------------------------

.PHONY: all clean

all: $(BINARIES)

$(OBJECTS): %.o: $(SRC_DIR)%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) $< -o $(OBJ_DIR)$@

$(BINARIES): %.elf: main_%.o
	$(CXX) $(CXXFLAGS) $(OBJ_DIR)$< -o $(BIN_DIR)$@

clean:
	$(RM) $(OBJECTS)
	$(RM) $(BIN_DIR)*.elf
