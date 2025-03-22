
cmake_minimum_required(VERSION 3.16)

project(overlay-base LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(overlay 
    main.cpp

    src/overlay/overlay.cpp 
    src/overlay/uiaccess/uiaccess.cpp

    dependencies/imgui.cpp
    dependencies/imgui_demo.cpp
    dependencies/imgui_draw.cpp
    dependencies/imgui_widgets.cpp
    dependencies/imgui_tables.cpp
    dependencies/imgui_impl_win32.cpp
    dependencies/imgui_impl_dx11.cpp
)

target_include_directories(overlay PRIVATE "$ENV{DXSDK_DIR}/Include")
target_link_directories(overlay PRIVATE "$ENV{DXSDK_DIR}/Lib/x64")
target_link_libraries(overlay PRIVATE d3d11 dxgi d3dcompiler)
