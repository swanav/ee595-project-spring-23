cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(tcp_ml VERSION 1.0.0 LANGUAGES C CXX)

# C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Set output directories
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add the include directory
# include_directories(${PROJECT_SOURCE_DIR}/include)

# Add the source directory
# add_subdirectory(module)
add_subdirectory(driver)
