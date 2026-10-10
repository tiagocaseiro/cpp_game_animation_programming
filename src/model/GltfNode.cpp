#include "GltfNode.h"

#include <algorithm>

#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_gltf.h>

template <int NumSubDivisions>
static auto unitSphereVertices()
{
    static_assert(NumSubDivisions >= 8, "NumSubDivisions can't be 0");

    float constexpr step = (glm::pi<float>() * 2) / static_cast<float>(NumSubDivisions);

    std::array<float, NumSubDivisions * 3 * 2> result;
    for(int i = 0; i != NumSubDivisions; i++)
    {
        int const offset = 3 * i;

        result[offset]     = glm::cos(step * i);
        result[offset + 1] = glm::sin(step * i);
        result[offset + 2] = 0.0f;

        result[(NumSubDivisions * 3) + offset]     = glm::cos(step * i);
        result[(NumSubDivisions * 3) + offset + 1] = 0.0;
        result[(NumSubDivisions * 3) + offset + 2] = glm::sin(step * i);
    }
    return result;
}

static constexpr int subDivisions = 32;

static auto vertices = unitSphereVertices<subDivisions>();

void printWhitespace(std::ostream& os, const int width)
{
    for(int i = 0; i != width; i++)
    {
        os << " ";
    }
}

GltfNodeShared GltfNode::createNode(const GltfNodeShared parent, const int nodeNum, const tinygltf::Model& model,
                                    const std::vector<int>& nodeToJoint,
                                    const std::vector<glm::mat4>& inverseBindMatrices,
                                    std::vector<glm::mat4>& jointMatrices, std::vector<GltfNodeShared>& nodes)
{
    if(nodeNum == -1)
    {
        return nullptr;
    }

    GltfNodeShared node =
        GltfNodeShared(new GltfNode(parent, nodeNum, model, nodeToJoint, inverseBindMatrices, jointMatrices));

    nodes[nodeNum] = node;

    std::vector<int> childrenNodes = model.nodes[nodeNum].children;

    node->mChildNodes.reserve(childrenNodes.size());
    for(const int childNodeNum : model.nodes[nodeNum].children)
    {
        node->mChildNodes.push_back(GltfNodeShared(
            createNode(node, childNodeNum, model, nodeToJoint, inverseBindMatrices, jointMatrices, nodes)));
    }

    return node;
}

GltfNodeShared GltfNode::createNodeTree(const int nodeNum, const tinygltf::Model& model,
                                        const std::vector<int>& nodeToJoint,
                                        const std::vector<glm::mat4>& inverseBindMatrices,
                                        std::vector<glm::mat4>& jointMatrices, std::vector<GltfNodeShared>& nodes)
{
    if(nodeNum == -1)
    {
        return nullptr;
    }

    return createNode(nullptr, nodeNum, model, nodeToJoint, inverseBindMatrices, jointMatrices, nodes);
}

GltfNode::~GltfNode()
{
    glDeleteBuffers(1, &mDebugVBO);
    glDeleteBuffers(1, &mDebugVAO);
}

void GltfNode::blendRotation(glm::quat const& rotation, float blendFactor)
{
    float const factor = std::clamp(blendFactor, 0.0f, 1.0f);

    mBlendRotation = glm::normalize(glm::slerp(mRotation, rotation, factor));
}

void GltfNode::blendTranslation(glm::vec3 const& translation, float blendFactor)
{
    float const factor = std::clamp(blendFactor, 0.0f, 1.0f);

    mBlendTranslation = translation * factor + mTranslation * (1.0f - factor);
}

void GltfNode::blendScale(glm::vec3 const& scale, float blendFactor)
{
    float const factor = std::clamp(blendFactor, 0.0f, 1.0f);

    mBlendScale = scale * factor + mScale * (1.0f - factor);
}

void GltfNode::setRotation(glm::quat const& rotation)
{
    mRotation      = rotation;
    mBlendRotation = rotation;
}

void GltfNode::setTranslation(glm::vec3 const& translation)
{
    mTranslation      = translation;
    mBlendTranslation = translation;
}

void GltfNode::setScale(glm::vec3 const& scale)
{
    mScale      = scale;
    mBlendScale = scale;
}

void GltfNode::calculateLocalTransform()
{
    glm::mat4 const sMatrix = glm::scale(glm::mat4(1.0f), mBlendScale);
    glm::mat4 const rMatrix = glm::mat4_cast(mBlendRotation);
    glm::mat4 const tMatrix = glm::translate(glm::mat4(1.0f), mBlendTranslation);

    mLocalTransform = tMatrix * rMatrix * sMatrix;
}

void GltfNode::calculateTreeMatrices(const std::vector<int>& nodeToJoint,
                                     const std::vector<glm::mat4>& inverseBindMatrices, glm::mat4 const& parentMatrix,
                                     std::vector<glm::mat4>& jointMatrices)
{
    mNodeMatrix = parentMatrix * mLocalTransform;

    const int jointIndex = nodeToJoint[mNodeNum];

    jointMatrices[jointIndex] = mNodeMatrix * inverseBindMatrices[jointIndex];

    for(GltfNodeShared const& child : mChildNodes)
    {
        if(child)
        {
            child->calculateTreeMatrices(nodeToJoint, inverseBindMatrices, mNodeMatrix, jointMatrices);
        }
    }
}

GltfNode::GltfNode(const GltfNodeShared parent, const int nodeNum, const tinygltf::Model& model,
                   const std::vector<int>& nodeToJoint, const std::vector<glm::mat4>& inverseBindMatrices,
                   std::vector<glm::mat4>& jointMatrices)
    : mNodeNum(nodeNum), mNodeName(model.nodes[nodeNum].name)
{
    const tinygltf::Node& node = model.nodes[nodeNum];

    if(node.scale.empty() == false)
    {
        setScale(glm::make_vec3(node.scale.data()));
    }

    if(node.rotation.empty() == false)
    {
        setRotation(glm::make_quat(node.rotation.data()));
    }

    if(node.translation.empty() == false)
    {
        setTranslation(glm::make_vec3(node.translation.data()));
    }

    calculateLocalTransform();

    const glm::mat4 parentNodeMatrix = parent ? parent->mNodeMatrix : IDENTITY_TRANSFORM;

    mNodeMatrix = parentNodeMatrix * mLocalTransform;

    const int jointIndex = nodeToJoint[mNodeNum];

    jointMatrices[jointIndex] = mNodeMatrix * inverseBindMatrices[jointIndex];

    createDebugVertexBuffers();
}

void GltfNode::printNode(std::ostream& os, int depth) const
{
    printWhitespace(os, depth);

    os << mNodeNum << " " << mNodeName << std::endl;

    depth++;

    for(const auto& childNode : mChildNodes)
    {
        if(childNode == nullptr)
        {
            printWhitespace(os, depth);
            os << "null" << std::endl;
            continue;
        }
        childNode->printNode(os, depth);
    }
}

void GltfNode::createDebugVertexBuffers()
{
    glGenVertexArrays(1, &mDebugVAO);
    glGenBuffers(1, &mDebugVBO);

    glBindVertexArray(mDebugVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mDebugVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void GltfNode::drawDebug()
{
    glBindVertexArray(mDebugVAO);
    glDrawArrays(GL_LINE_LOOP, 0, subDivisions * 2);
    glBindVertexArray(0);
}

std::ostream& operator<<(std::ostream& os, const GltfNode& node)
{
    node.printNode(os, 0);
    return os;
}
