#include "UserInterface.h"

#include <string>

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "RenderData.h"

UserInterface::UserInterface(const OGLRenderData& renderData)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(renderData.rdWindow, true);
    static constexpr char const* glslVersion = "#version 460 core";
    ImGui_ImplOpenGL3_Init(glslVersion);
}

UserInterface::~UserInterface()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UserInterface::createFrame(OGLRenderData& renderData)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags imguiWindowFlags = 0;
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::Begin("Control", nullptr, imguiWindowFlags);

    static float newFps = 0.0f;
    if(renderData.rdFrameTime > 0.0f)
    {
        newFps = 1.0f / renderData.rdFrameTime;
    }

    framesPerSecond = averagingAlpha * framesPerSecond + (1 - averagingAlpha) * newFps;

    ImGui::Text("FPS:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(framesPerSecond).c_str());
    ImGui::Separator();

    ImGui::Text("UI Generation Time:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(renderData.rdUIGenerateTime).c_str());
    ImGui::SameLine();
    ImGui::Text("ms");

    ImGui::Text("Triangles:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(renderData.rdTriangleCount).c_str());

    ImGui::Text("Gltf Triangles:");
    ImGui::SameLine();
    ImGui::Text(std::to_string(renderData.rdGltfTriangleCount).c_str());

    ImGui::Text("View Azimuth:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdViewAzimuth).c_str());

    ImGui::Text("View Elevation:");
    ImGui::SameLine();
    ImGui::Text("%s", std::to_string(renderData.rdViewElevation).c_str());

    std::string windowDims = std::to_string(renderData.rdWidth) + "x" + std::to_string(renderData.rdHeight);
    ImGui::Text("Window Dimensions:");
    ImGui::SameLine();
    ImGui::Text(windowDims.c_str());

    ImGui::Text("Camera Position:");
    ImGui::SameLine();
    ImGui::Text("%s %s %s", std::to_string(renderData.rdCameraWorldPosition.x).c_str(),
                std::to_string(renderData.rdCameraWorldPosition.y).c_str(),
                std::to_string(renderData.rdCameraWorldPosition.z).c_str());

    if(ImGui::CollapsingHeader("gltf Animation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Clip ");
        ImGui::SameLine();
        ImGui::Combo("##AnimClip", &renderData.rdAnimClip, renderData.rdAnimationClipNames.data(),
                     renderData.rdAnimationClipNames.size());

        ImGui::Text("Clip Duration (ms): %f", renderData.rdAnimDurationMs);

        ImGui::Checkbox("Play Animation", &renderData.rdPlayAnimation);

        if(renderData.rdPlayAnimation == false)
        {
            ImGui::BeginDisabled();
        }

        ImGui::Text("Speed ");
        ImGui::SameLine();
        ImGui::SliderFloat("##ClipSpeed", &renderData.rdAnimSpeed, 0.0f, 2);

        if(renderData.rdPlayAnimation == false)
        {
            ImGui::EndDisabled();
        }

        if(renderData.rdPlayAnimation)
        {
            ImGui::BeginDisabled();
        }

        ImGui::Text("Timepos");
        ImGui::SameLine();
        ImGui::SliderFloat("##ClipPos", &renderData.rdAnimTimePosition, 0.0f, renderData.rdAnimEndTime);

        if(renderData.rdPlayAnimation)
        {
            ImGui::EndDisabled();
        }
    }

    if(ImGui::CollapsingHeader("gltf Blending", ImGuiTreeNodeFlags_DefaultOpen))
    {

        ImGui::Checkbox("Blending Type: ", &renderData.rdCrossBlending);
        ImGui::SameLine();
        if(renderData.rdCrossBlending)
        {
            ImGui::Text("Cross");
        }
        else
        {
            ImGui::Text("Single");
        }

        if(renderData.rdCrossBlending)
        {
            ImGui::BeginDisabled();
        }

        ImGui::Text("Blend Factor");
        ImGui::SameLine();
        ImGui::SliderFloat("##BlendFactor", &renderData.rdAnimBlendFactor, 0.0f, 1.0);

        if(renderData.rdCrossBlending)
        {
            ImGui::EndDisabled();
        }

        if(renderData.rdCrossBlending == false)
        {
            ImGui::BeginDisabled();
        }

        ImGui::Text("DestClip ");
        ImGui::SameLine();
        ImGui::Combo("##DestClip", &renderData.rdCrossBlendDestAnimClip, renderData.rdAnimationClipNames.data(),
                     renderData.rdAnimationClipNames.size());

        ImGui::Text("Clip Duration (ms): %f", renderData.rdCrossBlendDestAnimDurationMs);

        ImGui::Text("Cross Blend Factor");
        ImGui::SameLine();
        ImGui::SliderFloat("##CrossBlendFactor", &renderData.rdAnimCrossBlendFactor, 0.0f, 1.0);

        if(renderData.rdCrossBlending == false)
        {
            ImGui::EndDisabled();
        }

        ImGui::Checkbox("Additive Blending: ", &renderData.rdAdditiveBlending);

        if(renderData.rdAdditiveBlending == false)
        {
            ImGui::BeginDisabled();
        }

        ImGui::Text("Split Node: ");
        ImGui::SameLine();
        ImGui::SliderInt("##SplitNode", &renderData.rdSkelSplitNode, 0, renderData.rdNodeCount - 1);
        ImGui::Text("Split Node Name: %s", renderData.rdSkelSplitNodeName.c_str());

        if(renderData.rdAdditiveBlending == false)
        {
            ImGui::EndDisabled();
        }
    }

    ImGui::End();
}

void UserInterface::render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
