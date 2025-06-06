# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude
LDLIBS = -lyaml-cpp

# Files and directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
EXE = $(BIN_DIR)/main

all: $(EXE)

$(EXE): $(OBJ_FILES) $(OBJ_DIR)/main.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/main

.PHONY: all clean
