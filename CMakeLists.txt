cmake_minimum_required(VERSION 3.20)

project(OpenGLRaytracing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -g")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
# set the opengl version to 4.6, and set to prefer newer libraries
set(OPENGL_GL_VERSION "4.6")
set(OPENGL_GLU_VERSION "4.6")
set(OPENGL_GL_PREFER_NEWER_LIBRARIES ON)
set(OPENGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

add_executable(OpenGLRaytracing ${SRCS})
target_include_directories(OpenGLRaytracing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include/)
target_link_libraries(OpenGLRaytracing glfw OpenGL::GL)

