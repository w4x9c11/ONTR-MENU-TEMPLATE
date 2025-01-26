cmake_minimum_required(VERSION 3.20)
project(ONTR-MENU-TEMPLATE)

if(MSVC)
    add_compile_options(/utf-8)
    
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/ONTR-MENU/Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/ONTR-MENU/Release")

# 源文件收集
file(GLOB CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/Core.cpp")
file(GLOB GUI_CORE_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/DeviceIO/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/ImGui-UI/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/Layouts/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/Layouts/Verify-Gui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/VendorModle/*.cpp" # lib实现模块
)

# 添加可执行文件
add_executable(ONTR-MENU
    ${CORE_SOURCES}
    ${GUI_CORE_SOURCES}
)

# 包含目录
target_include_directories(ONTR-MENU PRIVATE
    # 菜单GUI界面
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/ImGui-UI
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/DeviceIO
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/Elements
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/GUI/Languages
    # lib模块
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/VendorModle
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/VendorModle/Resources
    # lib库
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/nlohmann
    # ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/DirectXTK
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/stb

    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/NVIDIA-SDK
    # ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/KeyAuth
    # "E:/IDE/MSVC/IDE/VC/Tools/MSVC/14.42.34433/atlmfc/include"
)

# 链接库
target_link_libraries(ONTR-MENU PRIVATE
    d3d11.lib
    # dxgi.lib
    # d3dcompiler.lib
    # dxguid.lib
    # dwmapi.lib
    winhttp.lib
    # Ws2_32.lib
    # Shlwapi.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/NVIDIA-SDK/nvapi64.lib
    # ${CMAKE_CURRENT_SOURCE_DIR}/ONTR-MENU/lib/DirectXTK/DirectXTK.lib

    # "E:/IDE/MSVC/IDE/VC/Tools/MSVC/14.42.34433/atlmfc/lib/x64/atls.lib"
)

# 添加 ATL 头文件路径
target_include_directories(ONTR-MENU PRIVATE "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/atlmfc/include")

target_compile_features(ONTR-MENU PRIVATE cxx_std_20)

# 输出目录设置
set_target_properties(ONTR-MENU PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/ONTR-MENU/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/ONTR-MENU/Release"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/ONTR-MENU/RelWithDebInfo"
)

option(USE_NINJA "Use Ninja as the build system" ON)