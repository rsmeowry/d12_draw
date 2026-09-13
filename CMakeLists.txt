cmake_minimum_required(VERSION 3.20)
project(d12_draw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT WIN32)
    message(FATAL_ERROR "DirectX 12 requires Windows.")
endif()

add_executable(d12_draw WIN32
        src/main.cpp
        src/Window.cpp
        src/Window.h
        src/Util.h
)

target_include_directories(d12_draw PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external
)

target_link_libraries(d12_draw PRIVATE
        d3d12.lib
        dxgi.lib
        d3dcompiler.lib
)

# for debugging
target_compile_definitions(d12_draw PRIVATE
        $<$<CONFIG:Debug>:DX12_ENABLE_DEBUG_LAYER>
)