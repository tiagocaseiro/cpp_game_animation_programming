#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <tiny_gltf.h>

#include "GltfCommon.h"
#include "tools/Macros.h"

SHARED_ONLY(GltfNode);

namespace tinygltf
{
    class Model;
}

class GltfNode
{
public:
    static GltfNodeShared createNodeTree(const int nodeNum, const tinygltf::Model& model,
                                         const std::vector<int>& nodeToJoint,
                                         const std::vector<glm::mat4>& inverseBindMatrices,
                                         std::vector<glm::mat4>& mJointMatrices, std::vector<GltfNodeShared>& nodes);

    friend std::ostream& operator<<(std::ostream& os, const GltfNode& node);

    ~GltfNode();

    void blendRotation(glm::quat const& rotation, float blendFactor);
    void blendTranslation(glm::vec3 const& translation, float blendFactor);
    void blendScale(glm::vec3 const& scale, float blendFactor);

    void setRotation(glm::quat const& rotation);
    void setTranslation(glm::vec3 const& translation);
    void setScale(glm::vec3 const& scale);

    glm::quat globalRotation() const;
    glm::quat localRotation() const;
    glm::vec3 globalPosition() const;

    void calculateNodeMatrices();

    void calculateLocalTransform();

    void calculateTreeMatrices(const std::vector<int>& nodeToJoint, const std::vector<glm::mat4>& inverseBindMatrices,
                               std::vector<glm::mat4>& jointMatrices);

    void drawDebug();

    glm::mat4 worldTransform() const { return mNodeMatrix; }

    std::vector<GltfNodeShared> const& children() const { return mChildNodes; }

    GltfNodeShared ParentNode() const { return mParentNode.lock(); };

    const std::string mNodeName;
    const int mNodeNum;

private:
    static GltfNodeShared createNode(const std::shared_ptr<GltfNode> parent, const int nodeNum,
                                     const tinygltf::Model& model, const std::vector<int>& nodeToJoint,
                                     const std::vector<glm::mat4>& inverseBindMatrices,
                                     std::vector<glm::mat4>& mJointMatrices, std::vector<GltfNodeShared>& nodes);

    GltfNode(const GltfNodeShared parent, const int nodeNum, const tinygltf::Model& model,
             const std::vector<int>& nodeToJoint, const std::vector<glm::mat4>& inverseBindMatrices,
             std::vector<glm::mat4>& jointMatrices);

    void createDebugVertexBuffers();

    void calculateNodeMatrix();

    void printNode(std::ostream& os, int depth) const;

    std::vector<GltfNodeShared> mChildNodes;

    glm::vec3 mBlendScale       = DEFAULT_SCALE;
    glm::vec3 mBlendTranslation = DEFAULT_TRANSLATION;
    glm::quat mBlendRotation    = DEFAULT_ROTATION;

    glm::vec3 mScale       = DEFAULT_SCALE;
    glm::vec3 mTranslation = DEFAULT_TRANSLATION;
    glm::quat mRotation    = DEFAULT_ROTATION;

    glm::mat4 mLocalTransform = glm::mat4(1.0f);
    glm::mat4 mNodeMatrix     = glm::mat4(1.0f);

    GLuint mDebugVBO = 0;
    GLuint mDebugVAO = 0;

    const GltfNodeWeak mParentNode;
};

std::ostream& operator<<(std::ostream& os, const GltfNode& node);
