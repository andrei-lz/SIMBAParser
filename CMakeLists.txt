cmake_minimum_required(VERSION 3.12)
project(PCAPParser)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler flags for Debug and Release
if(MSVC)
    # Use static runtime (MT) and other common flags for MSVC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT /O2 /DNDEBUG")
else()
    # GCC/Clang universal flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto -funroll-loops -DNDEBUG")
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/src)

# Source files
file(GLOB_RECURSE SRC_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Add executable
add_executable(PCAPParser ${SRC_SOURCES})

# Print build configuration details
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Target architecture: ${CMAKE_GENERATOR_PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")