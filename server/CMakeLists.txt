cmake_minimum_required(VERSION 3.22)
project(Sledge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

include(ExternalProject)

set(BOOST_URL "https://archives.boost.io/release/1.85.0/source/boost_1_85_0.tar.gz")
set(BOOST_ROOT "${CMAKE_BINARY_DIR}/boost")

ExternalProject_Add(
    boost
    URL ${BOOST_URL}
    URL_HASH SHA256=be0d91732d5b0cc6fbb275c7939974457e79b54d6f07ce2e3dfdd68bef883b0b
    PREFIX ${BOOST_ROOT}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

set(BOX2D_ROOT "${CMAKE_BINARY_DIR}/box2d")
ExternalProject_Add(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v2.4.1
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${BOX2D_ROOT} -DBOX2D_BUILD_UNIT_TESTS=OFF -DBOX2D_BUILD_TESTBED=OFF
)

file(GLOB SOURCES "src/*.cpp" "src/*.h")

add_executable(sledge ${SOURCES})

include_directories(${BOOST_ROOT}/src/boost)
include_directories(${BOX2D_ROOT}/include)
include_directories(${CMAKE_SOURCE_DIR}/includes)

link_directories(${BOX2D_ROOT}/lib)

if(WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/libs)
    target_link_libraries(sledge debug ${BOX2D_ROOT}/lib/box2d.lib)
    target_link_libraries(sledge debug ${CMAKE_SOURCE_DIR}/libs/glfw/debug/glfw3.lib)
    target_link_libraries(sledge ${BOX2D_ROOT}/lib/box2d.lib)
    target_link_libraries(sledge ${CMAKE_SOURCE_DIR}/libs/glfw/release/glfw3.lib)
elseif(UNIX)
    target_link_libraries(sledge ${BOX2D_ROOT}/lib/libbox2d.a)
endif()