#pragma once

#include <vector>

// OpenGL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
    enum class Movement
    {
        FORWARD, BACKWARD, LEFT, RIGHT
    };

    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;

    // Eular Angles
    GLfloat yaw = -90.0f;
    GLfloat pitch = 0.0f;

    // Camera options
    GLfloat move_speed = 3.0f;
    GLfloat look_speed = 0.005f;
    GLfloat zoom_speed = 0.05f;

    // Constructor with vectors
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
    : position(pos),
      world_up(up)
    {
        updateVectors();
    }

    // Constructor with scalar values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ)
    : position(glm::vec3(posX, posY, posZ)),
      world_up(glm::vec3(upX, upY, upZ))
    {
        updateVectors();
    }

    // Return the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 view()
    {
        return glm::lookAt(position, position + front, up);
    }

    // Return the zoom in radians, rather than degrees
    GLfloat zoom()
    {
        return glm::radians(m_zoom);
    }

    void process_keyboard(Movement direction)
    {
        if (direction == Movement::FORWARD)
            position += front * move_speed;
        if (direction == Movement::BACKWARD)
            position -= front * move_speed;
        if (direction == Movement::LEFT)
            position -= right * move_speed;
        if (direction == Movement::RIGHT)
            position += right * move_speed;
    }

    void process_mouse(GLfloat xoffset, GLfloat yoffset)
    {
        xoffset *= look_speed;
        yoffset *= look_speed;

        yaw   += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        updateVectors();
    }

    void process_scroll(GLfloat yoffset)
    {
        if (m_zoom >= 1.0f && m_zoom <= 45.0f)
            m_zoom -= yoffset;
        if (m_zoom <= 1.0f)
            m_zoom = 1.0f;
        if (m_zoom >= 45.0f)
            m_zoom = 45.0f;
    }

private:
    GLfloat m_zoom = 45.0f;

    // Calculates the front vector from the Camera's Eular Angles
    void updateVectors()
    {
        glm::vec3 front;
        front.x = glm::cos(pitch) * glm::cos(yaw);
        front.y = glm::sin(pitch);
        front.z = glm::cos(pitch) * glm::sin(yaw);
        this->front = glm::normalize(front);
        this->right = glm::normalize(glm::cross(front, world_up));
        this->up    = glm::normalize(glm::cross(right, front));
    }
};
