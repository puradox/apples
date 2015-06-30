#include <cstring>
#include <cmath>

#include "graphics.hpp"
#include "script.hpp"
#include "cew.hpp"
#include "Shader.hpp"

int main()
{
    // =================================================================
    // LUA
    lua_State* L = lua_open();
    luaL_openlibs(L);
    luaL_dofile(L, "scripts/init.lua");

    // Retrieve some data
    lua_getglobal(L, "position");
    if (!lua_istable(L, -1))
        throw std::runtime_error("'vertex' is not a valid table");
    double x = getindex(L, "x");
    double y = getindex(L, "y");
    std::cout << x << std::endl;
    std::cout << y << std::endl;
    lua_pop(L, 1);

    // Draw triangle
    struct vertex {
        float x, y, z, r, g, b, u ,v;
    };
    lua_getglobal(L, "vertices");
    if (!lua_istable(L, -1))
        throw std::runtime_error("'vertices' is not a valid table");
    const int size = getindex(L, "size");
    vertex vertices[size];
    for (int i = 0; i < size; i++)
    {
        lua_rawgeti(L, 1, i+1);
        vertices[i].x = getindex(L, "x");
        vertices[i].y = getindex(L, "y");
        vertices[i].r = getindex(L, "r");
        vertices[i].g = getindex(L, "g");
        vertices[i].b = getindex(L, "b");
        std::cout << i << ": { x=" << vertices[i].x << ", y=" << vertices[i].y << " }\n";
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    std::cout << L << std::endl;
    lua_close(L);

    // GLFW - Set callbacks
    glfwSetErrorCallback(error_callback);

    // GLFW - Initialize
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // GLFW - Create window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "apples", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    glViewport(0, 0, WIDTH, HEIGHT);

    // GLEW - Initialize
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // OpenGL - Initialize
    GLfloat triangle[] = {
        // Positions         // Colors          // Texture Coords
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // Top Right
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // Bottom Left
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f   // Top Left
    };
    GLint indices[] = {
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GL_FLOAT)));
        glEnableVertexAttribArray(1);
        // Texture Coords attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GL_FLOAT)));
        glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    Shader defaultShader("scripts/default.vertex", "scripts/default.fragment");

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    GLuint textures[2];
    glGenTextures(2, textures);

    stbi_set_flip_vertically_on_load(1);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int width, height, bpp;
        unsigned char* image = stbi_load("scripts/container.jpg", &width, &height, &bpp, 3);
        if (!image)
            throw std::runtime_error(stbi_failure_reason());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int width, height, bpp;
        unsigned char* image = stbi_load("scripts/awesomeface.png", &width, &height, &bpp, 0);
        if (!image)
            throw std::runtime_error(stbi_failure_reason());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << (int)GL_ALPHA << std::endl;
    std::cout << (int)GL_RGB << std::endl;
    std::cout << (int)GL_RGBA << std::endl;
    std::cout << (int)GL_LUMINANCE << std::endl;
    std::cout << (int)GL_LUMINANCE_ALPHA << std::endl;

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

    // GLFW - Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0]);
            glUniform1i(glGetUniformLocation(defaultShader.m_program, "ourTexture1"), 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textures[1]);
            glUniform1i(glGetUniformLocation(defaultShader.m_program, "ourTexture2"), 1);

            GLfloat time = glfwGetTime();
            glUniform2f(glGetUniformLocation(defaultShader.m_program, "offset"), sin(time), cos(time));

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        Shader::release();

        glfwSwapBuffers(window);
    }

    // OpenGL - Cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    // GLFW - Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
}
