cmake_minimum_required(VERSION 3.31.6)
project(MirageC)

set(CMAKE_CXX_STANDARD 26)

add_executable(MirageC
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/codegen/codegen.cpp
)

target_include_directories(MirageC PRIVATE ${CMAKE_SOURCE_DIR}/src)
