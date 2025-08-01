cmake_minimum_required(VERSION 3.25)
project(MotionStaker)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Debug build")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    #set(CMAKE_EXE_LINKER_FLAGS "-static")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Release build")
    #set(CMAKE_EXE_LINKER_FLAGS "-static -mwindows")
    set(CMAKE_EXE_LINKER_FLAGS "-mwindows")
endif()

add_subdirectory(libs/raylib)

add_executable(MotionStaker src/main.cpp)

target_include_directories(MotionStaker PUBLIC libs/raylib/src)
target_include_directories(MotionStaker PUBLIC libs/raygui/src)

if (UNIX)
    message(STATUS "Linux platform")
    target_link_libraries(MotionStaker PUBLIC raylib)
endif (UNIX)

if (WIN32)
    message(STATUS "Windows platform")
    target_link_libraries(MotionStaker PUBLIC raylib)
    target_link_libraries(MotionStaker PUBLIC gdi32 opengl32 imm32)
endif(WIN32)