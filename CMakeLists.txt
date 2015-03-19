cmake_minimum_required(VERSION 2.8)

# Project details.
project(apples)

set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_LEVEL 0)

# C++11 Flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# Add executable.
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
    src/main.cpp
)

# Look for OpenGL.
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Look for GLFW.
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

# Look for GLEW.
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

# Set necessary libs.
set(PROJECT_REQUIRED_LIBS
    ${GLFW_STATIC_LIBRARIES}
	${GLEW_LIBRARIES}
	${OPENGL_LIBRARY}
)

# Set entry point for Visual Studio.
IF(MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup ")
ENDIF(MSVC)

# Copy resources over to binary location.
file(COPY src/resources DESTINATION ${PROJECT_BINARY_DIR})

# Link OpenGL libraries.
target_link_libraries(${PROJECT_NAME} ${PROJECT_REQUIRED_LIBS})
