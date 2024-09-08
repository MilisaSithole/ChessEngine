# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Target executable
TARGET = $(BUILD_DIR)/chess_engine

# Include directories
INCLUDES = -I$(INC_DIR)

# Test executable
TEST_TARGET = $(BUILD_DIR)/test

# Rules
all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	clear

run: all
	clear
	./$(TARGET)

clean_test:
	rm -f $(TEST_TARGET)
	clear

test: $(TEST_TARGET)
	clear
	./$(TEST_TARGET)

$(TEST_TARGET): test.cpp $(filter-out $(BUILD_DIR)/main.o, $(OBJ_FILES))
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

.PHONY: all clean
