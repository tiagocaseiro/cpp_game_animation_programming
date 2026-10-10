#pragma once

#include "FrameBuffer.h"
#include "RenderData.h"
#include "Shader.h"
#include "ShaderStorageBuffer.h"
#include "UserInterface.h"
#include "VertexBuffer.h"
#include "model/GltfModel.h"
#include "tools/Camera.h"
#include "tools/Macros.h"
#include "tools/Timer.h"

struct GLFWWindow;
class GltfModel;
class UniformBuffer;
class Shader;

SHARED_ONLY(Renderer)

class Renderer
{
public:
    static RendererShared make(const int width, const int height, GLFWwindow* window);

    ~Renderer();

    void setSize(int const width, int const height);
    void uploadData(const OGLMesh& vertexData);
    void draw();
    void handleKeyEvents(const int key, const int scancode, const int action, const int mods);
    void handleMouseButtonEvents(const int button, const int action, const int mods);
    void handleMousePositionEvents(const double xPos, const double yPos);
    void handleMovementKeys();

private:
    Renderer(ShaderShared const& gltfShader, ShaderShared const& debugGltfShader, const Framebuffer& framebuffer,
             const std::shared_ptr<GltfModel>& gltfModel, const OGLRenderData& renderData);

    ShaderShared mGltfShader;
    ShaderShared mDebugGltfShader;
    Framebuffer mFramebuffer;
    VertexBuffer mVertexBuffer;
    std::shared_ptr<UniformBuffer> mUniformBuffer;
    std::shared_ptr<ShaderStorageBuffer<glm::mat4>> mShaderStorageBufferJointMatrices;
    // std::shared_ptr<ShaderStorageBuffer<glm::mat2x4>> mShaderStorageBufferJointDualQuats;
    GltfModelShared mGltfModel;

    glm::mat4 mViewMatrix       = glm::mat4(1.0);
    glm::mat4 mProjectionMatrix = glm::mat4(1.0);

    OGLRenderData mRenderData;
    UserInterface mUserInterface;
    Timer mUIGenerateTimer;
    Camera mCamera;

    bool mMouseLock     = false;
    int mMouseXPos      = 0;
    int mMouseYPos      = 0;
    float mLastTickTime = 0;
};