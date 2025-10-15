cmake_minimum_required(VERSION 3.16)
project(PyramidGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXTERNAL "${CMAKE_SOURCE_DIR}/external")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
add_subdirectory("${EXTERNAL}/glfw")

add_library(glad ${EXTERNAL}/glad/src/glad.c)
target_include_directories(glad PUBLIC "${EXTERNAL}/glad/include")

set(SOURCES
    "${SRC_DIR}/main.cpp"
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw glad GLX)
