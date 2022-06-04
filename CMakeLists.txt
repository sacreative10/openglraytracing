cmake_minimum_required(VERSION 3.0)

project(OpenGLRaytracing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -g")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_executable(OpenGLRaytracing ${SRCS})
target_include_directories(OpenGLRaytracing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include/)
target_link_libraries(OpenGLRaytracing glfw OpenGL::GL)

