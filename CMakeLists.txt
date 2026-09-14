cmake_minimum_required(VERSION 3.20)
project(d12_draw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT WIN32)
    message(FATAL_ERROR "DirectX 12 requires Windows.")
endif()

add_executable(d12_draw WIN32
        src/main.cpp
        src/GameWindow.cpp
        src/GameWindow.h
        src/Util.h
        src/DXFramework.cpp
        src/DXFramework.h
        include/Delegates.cpp
        include/Delegates.h
        src/InputDevice.cpp
        src/D3DApp.cpp
        src/D3DApp.h
        src/Timer.cpp
        src/Timer.h
        src/GraphicsDevice.cpp
        src/GraphicsDevice.h
        src/MyApp.cpp
        src/MyApp.h
)

target_include_directories(d12_draw PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/external
        ${CMAKE_SOURCE_DIR}/external/DirectX-Headers/include/directx
)

find_package(directxtk12 CONFIG REQUIRED)

target_link_libraries(d12_draw PRIVATE
        Microsoft::DirectXTK12

        d3d12.lib
        dxgi.lib
        d3dcompiler.lib
)

target_compile_definitions(d12_draw PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)
target_compile_definitions(d12_draw PRIVATE UNICODE _UNICODE)
target_compile_definitions(d12_draw PRIVATE
        $<$<CONFIG:Debug>:DX12_ENABLE_DEBUG_LAYER>
)