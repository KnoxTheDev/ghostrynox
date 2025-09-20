cmake_minimum_required(VERSION 3.10)
project(Rynox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES src/Rynox.cpp)

# Create shared library
add_library(Rynox SHARED ${SOURCES})

# Include directories
target_include_directories(Rynox
        PUBLIC
        src
        ${CMAKE_SOURCE_DIR}/include
)

# Link against JVM and pthread
target_link_libraries(Rynox
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include/libjvm.dylib
        pthread
)
