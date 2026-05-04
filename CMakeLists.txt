cmake_minimum_required(VERSION 3.10)

project(ChessGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/raylib)

include_directories(include)
include_directories(external/raylib/src)

file(GLOB SOURCES 
    "main.cpp" 
    "src/*.cpp"
)

add_executable(ChessGame ${SOURCES})

target_link_libraries(ChessGame raylib)