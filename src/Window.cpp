#include "Window.h"

#include <memory>

#include <GLFW/glfw3.h>

#include "Logger.h"

bool Window::init(const int width, const int height, const std::string& title)
{
    if(glfwInit() == false)
    {
        Logger::log(1, "%s glfwInit() error\n", __FUNCTION__);
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if(mWindow == nullptr)
    {
        Logger::log(1, "%s: Could not create window\n", __FUNCTION__);
        glfwTerminate();
        return false;
    }

    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    if(count > 1)
    {
        int xpos, ypos;
        glfwGetMonitorPos(monitors[1], &xpos, &ypos);

        glfwSetWindowPos(mWindow, xpos + 400, ypos + 50);
    }

    glfwMakeContextCurrent(mWindow);

    glfwSetWindowUserPointer(mWindow, this);

    glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window) {
        Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        thisWindow->handleWindowCloseEvents();
    });

    mRenderer = Renderer::make(width, height, mWindow);

    if(mRenderer == nullptr)
    {
        Logger::log(1, "%s: Failed to initialize renderer\n", __FUNCTION__);
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(mWindow, this);

    glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) {
        Window* mWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        mWindow->getRenderer()->handleMouseButtonEvents(button, action, mods);
    });

    glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos) {
        Window* mWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        mWindow->getRenderer()->handleMousePositionEvents(xPos, yPos);
    });

    glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        Window* mWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        mWindow->handleKeyEvents(key, scancode, action, mods);
    });

    glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
        Window* mWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        mWindow->getRenderer()->setSize(width, height);
    });

    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    Logger::log(1, "%s: Window successfully initialized\n", __FUNCTION__);
    return true;
}

void Window::mainLoop()
{
    glfwSwapInterval(1);

    while(glfwWindowShouldClose(mWindow) == false)
    {
        mRenderer->draw();
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
}

void Window::cleanup()
{
    Logger::log(1, "%s: Terminating window\n", __FUNCTION__);
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void Window::handleWindowCloseEvents() { Logger::log(1, "%s: Window got a close event\n", __FUNCTION__); }

void Window::handleKeyEvents(const int key, const int scancode, const int action, const int mods)
{
    std::string actionName;

    switch(action)
    {
        case GLFW_RELEASE:
            actionName = "released";
            if(key == GLFW_KEY_ESCAPE)
            {
                glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
            }
            break;
        case GLFW_PRESS:
            actionName = "press";
            break;
        case GLFW_REPEAT:
            actionName = "repeat";
            break;
        default:
            actionName = "invalid";
            break;
    }

    const char* keyName = glfwGetKeyName(key, 0);

    mRenderer->handleKeyEvents(key, scancode, action, mods);
}
