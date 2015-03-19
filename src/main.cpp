#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLSL(src) "#version 150 core\n" #src

static void error_callback(int error, const char* description)
{
    std::cerr << "ERROR " << error << " - " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
    // Set error callback
    glfwSetErrorCallback(error_callback);

    // Init GLFW
    if (!glfwInit())
        return -1;

    // Create a rendering window with OpenGL 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(800, 600, "apples", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Configure window
    glfwMakeContextCurrent(window);

    // Set key callback
    glfwSetKeyCallback(window, key_callback);

    // Init GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        return -1;
    }

    // Create a Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);         // Generate 1 vertex array
    glBindVertexArray(vao);             // Bind

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);              // Generate 1 buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Make into active object

    float vertices[] = {
         0.0f,  0.5f,   // Vertex 1 (X, Y)
         0.5f, -0.5f,   // Vertex 2 (X, Y)
        -0.5f, -0.5f    // Vertex 3 (X, Y)
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Types of OpenGL draws
    // GL_STATIC_DRAW: The vertex data will be uploaded once and drawn many times (e.g. the world).
    // GL_DYNAMIC_DRAW: The vertex data will be changed from time to time, but drawn many times more than that.
    // GL_STREAM_DRAW: The vertex data will change almost every time it's drawn (e.g. user interface).

    // Create vertex shader
    const char* vertexSource = GLSL(
        in vec2 position;

        void main() {
            gl_Position = vec4(position, 0.0, 1.0);
        }
    );

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Create fragment shader
    const char* fragmentSource = GLSL(
        out vec4 outColor;

        void main() {
            outColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    );

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Combine shaders into a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Link vertex data and attributes
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    // Rendering
    while (!glfwWindowShouldClose(window))
    {
        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle from the 3 vertices
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers and poll window events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);

    // Terminate GLFW
    glfwTerminate();
    return 0;
}
