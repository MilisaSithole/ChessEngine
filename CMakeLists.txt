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

# Set CUDA compiler path
set(CUDACXX /usr/bin/nvcc)

# Use CUDACXX as the CUDA compiler
set(CMAKE_CUDA_COMPILER ${CUDACXX})

# Find libtorch package
# set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/home/mili/Downloads/libtorch-cxx11-abi-shared-with-deps-2.4.1+cpu/libtorch")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/home/mili/Downloads/libtorch-cxx11-abi-shared-with-deps-2.4.1+cu124/libtorch")
find_package(Torch REQUIRED)

# Find CUDA package (ensure CUDA is found)
find_package(CUDA REQUIRED)

# Set RPATH to point to CUDA libraries
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH ${CMAKE_PREFIX_PATH} /usr/local/cuda/lib64)

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