cmake_minimum_required(VERSION 3.5)
project(ChessEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
add_compile_options(-Wall -Wextra)

# Define source and include directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Include directories
include_directories(${INC_DIR})

# Find source files
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")

# Find libtorch package
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/home/mili/Downloads/libtorch-cxx11-abi-shared-with-deps-2.4.1+cpu/libtorch")
find_package(Torch REQUIRED)

# Create main exe
add_executable(ChessEngine ${SOURCES})
target_link_libraries(ChessEngine ${TORCH_LIBRARIES})

# Custom targets
add_custom_target(run
    COMMAND ChessEngine
    DEPENDS ChessEngine
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Set C++ flags for libtorch
set_property(TARGET ChessEngine PROPERTY CXX_STANDARD 17)