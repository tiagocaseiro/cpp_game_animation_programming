#include <memory>

#include "Window.h"
#include "tools/Logger.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>

int main()
{
    std::unique_ptr<Window> w = std::make_unique<Window>();

    if(w->init(1280, 960, "Test Window") == false)
    {
        Logger::log(1, "%s error: Window init error\n", __FUNCTION__);
        return -1;
    }

    w->mainLoop();
    w->cleanup();

    return 0;
}