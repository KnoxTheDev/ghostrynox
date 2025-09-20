cmake_minimum_required(VERSION 3.10)
project(GhostClientCore CXX)

# Set C++ standard to C++17 for modern features and enable position-independent code
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Define the output as a shared library (dylib on macOS)
# We will add source files here as we create them.
add_library(GhostClientCore SHARED
    src/main.cpp
    src/core/module.cpp
    src/core/module_manager.cpp
    src/modules/sprint.cpp
    src/core/jni/jni_helper.cpp
    src/core/hooking/hook.cpp
    src/gui/GuiController.mm
    src/core/self_destruct.cpp
)

# Add architecture-specific source files
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    target_sources(GhostClientCore PRIVATE src/core/hooking/hook_x86_64.cpp)
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    # In the future, we would add the arm64 implementation here
    # target_sources(GhostClientCore PRIVATE src/core/hooking/hook_arm64.cpp)
    message(WARNING "ARM64 hooking is not yet implemented.")
endif()

# Specify include directories
target_include_directories(GhostClientCore PUBLIC include)

# Link against required macOS frameworks for Mach IPC
if(APPLE)
    target_link_libraries(GhostClientCore PRIVATE "-framework Foundation" "-framework Cocoa")
    # Enable Objective-C++ compilation for .mm files
    set_source_files_properties(src/gui/GuiController.mm PROPERTIES
        COMPILE_FLAGS "-x objective-c++")
endif()

# Set the output name and extension to be clean
set_target_properties(GhostClientCore PROPERTIES
    PREFIX ""
    SUFFIX .dylib
)
