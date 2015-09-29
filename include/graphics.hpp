#pragma once

// STD C++
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

// OpenGL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// STB
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Project
#include "Camera.hpp"


const GLuint WIDTH = 800, HEIGHT = 600;

const GLfloat SEC_PER_UPDATE = 1.0f / 60.0f;

bool keys[1024];

GLfloat lastX = WIDTH / 2;
GLfloat lastY = HEIGHT / 2;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

static void error_callback(int error, const char* description)
{
    std::cerr << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
}

bool firstMouse = true;
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.process_mouse(xoffset, yoffset);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.process_scroll(yoffset);
}

void update_movement()
{
    if (keys[GLFW_KEY_W])
        camera.process_keyboard(Camera::Movement::FORWARD);
    if (keys[GLFW_KEY_S])
        camera.process_keyboard(Camera::Movement::BACKWARD);
    if (keys[GLFW_KEY_A])
        camera.process_keyboard(Camera::Movement::LEFT);
    if (keys[GLFW_KEY_D])
        camera.process_keyboard(Camera::Movement::RIGHT);
}
