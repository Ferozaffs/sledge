cmake_minimum_required(VERSION 3.29)
project(Sledge)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

file(GLOB SOURCES "src/*.cpp" "src/*.h")

add_executable(sledge ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/includes)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})

link_directories(${CMAKE_SOURCE_DIR}/libs)

target_link_libraries(sledge debug ${CMAKE_SOURCE_DIR}/libs/box2d/debug/box2d.lib)
target_link_libraries(sledge debug ${CMAKE_SOURCE_DIR}/libs/glfw/debug/glfw3.lib)
target_link_libraries(sledge ${CMAKE_SOURCE_DIR}/libs/box2d/release/box2d.lib)
target_link_libraries(sledge ${CMAKE_SOURCE_DIR}/libs/glfw/release/glfw3.lib)

