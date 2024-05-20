cmake_minimum_required(VERSION 3.22)
project(Sledge)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

include(ExternalProject)

set(BOOST_URL "https://boostorg.jfrog.io/artifactory/main/release/1.85.0/source/boost_1_85_0.tar.gz")
set(BOOST_ROOT "${CMAKE_BINARY_DIR}/boost")
set(BOOST_INCLUDE_DIR "${BOOST_ROOT}/include")

ExternalProject_Add(
    boost
    URL ${BOOST_URL}
    URL_HASH SHA256=be0d91732d5b0cc6fbb275c7939974457e79b54d6f07ce2e3dfdd68bef883b0b
    PREFIX ${BOOST_ROOT}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

file(GLOB SOURCES "src/*.cpp" "src/*.h")

add_executable(sledge ${SOURCES})

include_directories(${BOOST_ROOT}/src/boost)
include_directories(${CMAKE_SOURCE_DIR}/includes)

link_directories(${CMAKE_SOURCE_DIR}/libs)

target_link_libraries(sledge debug ${CMAKE_SOURCE_DIR}/libs/box2d/debug/box2d.lib)
target_link_libraries(sledge debug ${CMAKE_SOURCE_DIR}/libs/glfw/debug/glfw3.lib)
target_link_libraries(sledge ${CMAKE_SOURCE_DIR}/libs/box2d/release/box2d.lib)
target_link_libraries(sledge ${CMAKE_SOURCE_DIR}/libs/glfw/release/glfw3.lib)

