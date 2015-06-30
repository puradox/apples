cmake_minimum_required(VERSION 2.8)

# Project details
project(apples)
include_directories(include)

# Include dependencies
include_directories(dep/LuaBridge/Source)
include_directories(dep/stb)

# CMake configuration
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb")
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})

# Look for OpenGL
find_package(OpenGL REQUIRED)

# Look for GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

# Look for GLFW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

# Look for LUA
find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

# Set necessary libs
set(REQUIRED_LIBS
    ${LUA_LIBRARIES}
    ${GLFW_STATIC_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARY}
    )

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS})
