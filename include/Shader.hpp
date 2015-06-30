#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>

class Shader
{
public:
    // Program ID
    GLuint m_program;

    // Reads and builds the shader
    Shader(const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath);

    // Use the program
    void use()
    {
        glUseProgram(m_program);
    }

    static void release()
    {
        glUseProgram(0);
    }
};

Shader::Shader(const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath)
{
    std::ifstream file;
    std::stringstream buffer;

    // Load vertex shader source code
    file.open(vertexSourcePath);
    if (!file)
    {
        std::cerr << "ERROR::SHADER::VERTEX::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        return;
    }
    buffer << file.rdbuf();
    std::string vertexString = buffer.str();
    const char* vertexSource = vertexString.c_str();
    file.close();

    // Clear buffer
    buffer.clear();
    buffer.str(std::string());

    // Load fragment shader source code
    file.open(fragmentSourcePath);
    if (!file)
    {
        std::cerr << "ERROR::SHADER::FRAGMENT::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        return;
    }
    buffer << file.rdbuf();
    std::string fragmentString = buffer.str();
    const char* fragmentSource = fragmentString.c_str();
    file.close();

    // Create the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar error[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, sizeof(error), NULL, error);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << error << std::endl;
    }

    // Create the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, sizeof(error), NULL, error);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << error << std::endl;
    }

    // Creat the shader program
    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_program, sizeof(error), NULL, error);
        std::cerr << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << error << std::endl;
    }

    // Cleanup
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
}
