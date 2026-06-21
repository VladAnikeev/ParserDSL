cmake_minimum_required(VERSION 3.10)
project(ParserDSL)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF)
set(PUGIXML_BUILD_EXAMPLES OFF)
set(PUGIXML_BUILD_TESTS OFF)
add_subdirectory(pugixml)

add_executable(parser 
    src/main.cpp
    src/block.cpp
    src/block_manager.cpp
    src/line.cpp
    src/line_manager.cpp
    src/dependency_graph.cpp
    src/code_generator.cpp
)

target_include_directories(parser PRIVATE src)
target_link_libraries(parser PRIVATE pugixml::pugixml)