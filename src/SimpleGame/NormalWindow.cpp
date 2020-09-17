#include "DemoWindows.h"

int ShowNormalWindow() 
{
    // glfwSetErrorCallback()

    if (!glfwInit()) 
    {
        return -1;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(900, 450, "Simple Window", nullptr, nullptr);

    if (window == nullptr)
    {
        return -2;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    bool err = (glewInit() != GLEW_OK);
    if (err) 
    {
        return -3;
    }

    while (!glfwWindowShouldClose(window))
    {
        glViewport(0, 0, 900, 450);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}