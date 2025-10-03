cmake_minimum_required(VERSION 3.20)
project(ONTR-MENU-TEMPLATE LANGUAGES CXX)

# ------------------------
# C++ Standard
# ------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # Generate compile_commands.json for clangd

# ------------------------
# Output directories
# ------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} UPPER_CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${UPPER_CONFIG} ${CMAKE_BINARY_DIR}/bin/${CONFIG})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${UPPER_CONFIG} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${UPPER_CONFIG} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
endforeach()

# ------------------------
# Source files
# ------------------------
file(GLOB_RECURSE CPP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/HACK/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/*.cpp
)

# ------------------------
# Add executable
# ------------------------
add_executable(ONTR-MENU ${CPP_SOURCES})

# ------------------------
# Include directories
# ------------------------
target_include_directories(ONTR-MENU PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/DeviceIO
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/Elements
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/Languages
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/Layouts

    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/HACK
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/HACK/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/HACK/SDK

    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/directxtk
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/nlohmann
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/NVIDIA-SDK
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/VendorModle
)

# ------------------------
# Link libraries
# ------------------------
target_link_libraries(ONTR-MENU PRIVATE
    d3d11.lib
    dxgi.lib
    winhttp.lib
    "${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/NVIDIA-SDK/nvapi64.lib"
)

# ------------------------
# MSVC-specific compile options
# ------------------------
if(MSVC)
    target_compile_options(ONTR-MENU PRIVATE
        /utf-8          # UTF-8 encoding
        /W4             # High warning level
        /permissive-    # Strict standard compliance
        /MP             # Parallel build
    )
endif()

# ------------------------
# Copy compile_commands.json to root for clangd
# ------------------------
add_custom_target(copy-compile-commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

# ------------------------
# Organize files in Visual Studio
# ------------------------
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU FILES ${CPP_SOURCES})