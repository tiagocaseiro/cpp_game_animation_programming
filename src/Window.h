#pragma once

#include <memory>
#include <string>

#include "opengl/Renderer.h"

struct GLFWwindow;

class Window
{
public:
    bool init(const int width, const int height, const std::string& title);
    void mainLoop();
    void cleanup();

    const RendererShared& getRenderer() { return mRenderer; }

private:
    void handleWindowCloseEvents();
    void handleKeyEvents(const int key, const int scancode, const int action, const int mods);

    GLFWwindow* mWindow = nullptr;

    RendererShared mRenderer;
};