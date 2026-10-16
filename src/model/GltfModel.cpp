#include "GltfModel.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <chrono>

#include <tiny_gltf.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "GltfNode.h"
#include "Logger.h"
#include "opengl/RenderData.h"

namespace chrono = std::chrono;

static const std::string_view ATTRIBUTES[] = {"POSITION", "NORMAL", "TEXCOORD_0", "JOINTS_0", "WEIGHTS_0"};

static void initializeFromBuffer(const tinygltf::Model& model, const int accessorIndex, auto& destination)
{
    const tinygltf::Accessor& accessor     = model.accessors[accessorIndex];
    const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer         = model.buffers[bufferView.buffer];

    destination.resize(accessor.count);
    memcpy(destination.data(), buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);
}

static int getAttributeIndex(std::string_view attribute)
{
    return std::distance(std::begin(ATTRIBUTES), std::ranges::find(ATTRIBUTES, attribute));
}

GltfModelShared GltfModel::make(RenderData& renderData, const std::string& modelFilename,
                                const std::string& textureFilename)
{
    std::shared_ptr<Texture> tex = Texture::make(textureFilename, false);
    if(tex == nullptr)
    {
        return nullptr;
    }

    std::shared_ptr<tinygltf::Model> model = std::make_shared<tinygltf::Model>();
    tinygltf::TinyGLTF gltfLoader;
    std::string loaderErrors;
    std::string loaderWarnings;
    bool result = false;

    result = gltfLoader.LoadASCIIFromFile(model.get(), &loaderErrors, &loaderWarnings, modelFilename);

    if(loaderWarnings.empty() == false)
    {
        Logger::log(1, "%s: warnings while loading glTF model:\n%s\n", __FUNCTION__, loaderWarnings.c_str());
    }

    if(loaderWarnings.empty() == false)
    {
        Logger::log(1, "%s: errors while loading glTF model:\n%s\n", __FUNCTION__, loaderErrors.c_str());
    }

    if(result == false)
    {
        Logger::log(1, "%s error: while loading file %s\n", __FUNCTION__, modelFilename.c_str());
        return nullptr;
    }

    return GltfModelShared(new GltfModel(model, tex, renderData));
}

GltfModel::GltfModel(const std::shared_ptr<tinygltf::Model>& model, const std::shared_ptr<Texture>& tex,
                     RenderData& renderData)
    : mModel(model), mTex(tex)
{
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    createVertexBuffers();
    createIndexBuffer();
    glBindVertexArray(0);

    renderData.rdGltfTriangleCount = getTriangleCount();

    const std::vector<int>& nodes = mModel->scenes[0].nodes;
    const tinygltf::Skin& skin    = mModel->skins[0];

    int inverseBindMatricesIndex = skin.inverseBindMatrices;
    if(inverseBindMatricesIndex != -1)
    {
        initializeFromBuffer(*mModel, inverseBindMatricesIndex, mInverseBindMatrices);
    }

    mNodeToJoint.resize(mModel->nodes.size());

    const std::vector<int>& joints = skin.joints;
    for(int i = 0; i != joints.size(); i++)
    {
        const int nodeIndex     = joints[i];
        mNodeToJoint[nodeIndex] = i;
    }

    mJointMatrices.resize(mInverseBindMatrices.size());

    initializeNodes(renderData);

    std::cout << *mRootNode << std::endl;

    resetAdditiveMasks();

    // mJointDualQuats.resize(skin.joints.size());

    // for(int i = 0; i != mJointMatrices.size(); i++)
    // {
    //     glm::vec3 scale;
    //     glm::quat orientation;
    //     glm::vec3 translation;
    //     glm::vec3 skew;
    //     glm::vec4 perspective;

    //     if(glm::decompose(mJointMatrices[i], scale, orientation, translation, skew, perspective))
    //     {
    //         glm::dualquat dq;

    //         dq[0] = orientation;
    //         dq[1] = glm::quat(0.0, translation.x, translation.y, translation.z) * orientation * 0.5f;

    //         mJointDualQuats[i] = glm::mat2x4_cast(dq);
    //     }
    // }

    for(const auto& anim : mModel->animations)
    {
        renderData.rdAnimationClipNames.push_back(anim.name.c_str());
        mAnimClips.push_back(GltfAnimationClip::make(*mModel, anim));
    }

    renderData.rdAnimationClipSize = mAnimClips.size();
}

void GltfModel::draw()
{
    const tinygltf::Primitive& primitives   = mModel->meshes.front().primitives.front();
    const tinygltf::Accessor& indexAccessor = mModel->accessors[primitives.indices];

    GLuint drawMode = GL_TRIANGLES;
    switch(primitives.mode)
    {
        case TINYGLTF_MODE_TRIANGLES:
            drawMode = GL_TRIANGLES;
            break;
        default:
            Logger::log(1, "%s error: unknown draw mode %i\n", __FUNCTION__, drawMode);
            break;
    }

    mTex->bind();
    glBindVertexArray(mVAO);
    glDrawElements(drawMode, indexAccessor.count, indexAccessor.componentType, nullptr);

    glBindVertexArray(0);
    mTex->unbind();
}

GltfModel::~GltfModel()
{
    glDeleteBuffers(mVertexVBO.size(), mVertexVBO.data());
    glDeleteBuffers(1, &mVAO);
    glDeleteBuffers(1, &mIndexVBO);
}

void GltfModel::uploadVertexBuffers()
{
    for(int i = 0; i < std::size(ATTRIBUTES); i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[i]);
        const tinygltf::Accessor& accessor     = mModel->accessors[i];
        const tinygltf::BufferView& bufferView = mModel->bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer         = mModel->buffers[bufferView.buffer];

        glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength, &buffer.data[0] + bufferView.byteOffset, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void GltfModel::uploadIndexBuffer()
{
    const tinygltf::Primitive& primitives  = mModel->meshes.front().primitives.front();
    const tinygltf::Accessor& accessor     = mModel->accessors[primitives.indices];
    const tinygltf::BufferView& bufferView = mModel->bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer         = mModel->buffers[bufferView.buffer];

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferView.byteLength, &buffer.data[0] + bufferView.byteOffset,
                 GL_STATIC_DRAW);
}

void GltfModel::createVertexBuffers()
{
    const tinygltf::Primitive& primitives = mModel->meshes.front().primitives.front();
    mVertexVBO.resize(primitives.attributes.size());

    for(const auto& [attribType, accessorNum] : primitives.attributes)
    {
        if(std::ranges::contains(ATTRIBUTES, attribType) == false)
        {
            continue;
        }

        const tinygltf::Accessor& accessor = mModel->accessors[accessorNum];

        int dataSize = 1;
        switch(accessor.type)
        {
            case TINYGLTF_TYPE_SCALAR:
                dataSize = 1;
                break;
            case TINYGLTF_TYPE_VEC2:
                dataSize = 2;
                break;
            case TINYGLTF_TYPE_VEC3:
                dataSize = 3;
                break;
            case TINYGLTF_TYPE_VEC4:
                dataSize = 4;
                break;
            default:
                Logger::log(1, "%s error: accessor %i uses data size %i\n", __FUNCTION__, accessorNum, dataSize);
                break;
        }

        GLuint dataType = GL_FLOAT;
        switch(accessor.componentType)
        {
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                dataType = GL_FLOAT;
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                dataType = GL_UNSIGNED_SHORT;
                break;
            default:
                Logger::log(1, "%s error: accessor %i uses unknown data type %i\n", __FUNCTION__, accessorNum,
                            dataType);
                break;
        }

        const int attributeIndex = getAttributeIndex(attribType);

        glGenBuffers(1, &mVertexVBO[attributeIndex]);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[attributeIndex]);

        glVertexAttribPointer(attributeIndex, dataSize, dataType, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(attributeIndex);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void GltfModel::createIndexBuffer()
{
    glGenBuffers(1, &mIndexVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
}

int GltfModel::getTriangleCount() const
{
    const tinygltf::Primitive& primitives = mModel->meshes.front().primitives.front();
    const tinygltf::Accessor& accessor    = mModel->accessors[primitives.indices];
    return accessor.count;
}

void GltfModel::playAnimation(int animNum, float speedDivider, float blendFactor)
{
    chrono::steady_clock::duration const now = chrono::steady_clock::now().time_since_epoch();

    double const currentTime = chrono::duration_cast<chrono::milliseconds>(now).count();

    blendAnimationFrame(animNum, std::fmod(currentTime / 1000.0 * speedDivider, mAnimClips[animNum].getClipEndTime()),
                        blendFactor);
}

void GltfModel::playAnimation(int sourceAnimNum, int destAnimNum, float speedDivider, float blendFactor)
{
    chrono::steady_clock::duration const now = chrono::steady_clock::now().time_since_epoch();

    double const currentTime = chrono::duration_cast<chrono::milliseconds>(now).count();

    crossBlendAnimationFrame(sourceAnimNum, destAnimNum,
                             std::fmod(currentTime / 1000.0 * speedDivider, mAnimClips[sourceAnimNum].getClipEndTime()),
                             blendFactor);
}

std::string GltfModel::getClipName(int animNum) const
{
    if(animNum < mAnimClips.size())
    {
        return mAnimClips[animNum].mName;
    }

    return "Invalid";
}

float GltfModel::getClipEndTime(int animNum) const
{
    if(animNum < mAnimClips.size())
    {
        return mAnimClips[animNum].getClipEndTime();
    }

    return 0.0f;
}

void GltfModel::blendAnimationFrame(int animNum, float time, float blendFactor)
{
    GltfAnimationClip& animClip = mAnimClips[animNum];
    animClip.blendAnimationFrame(mNodes, mAdditiveAnimationMask, time, blendFactor);
    mRootNode->calculateTreeMatrices(mNodeToJoint, mInverseBindMatrices, mJointMatrices);
}

void GltfModel::crossBlendAnimationFrame(int sourceAnimNum, int destAnimNum, float time, float blendFactor)
{
    GltfAnimationClip const& sourceAnimClip = mAnimClips[sourceAnimNum];
    GltfAnimationClip const& destAnimClip   = mAnimClips[destAnimNum];

    float const sourceAnimDuration = sourceAnimClip.getClipEndTime();
    float const destAnimDuration   = destAnimClip.getClipEndTime();

    float const scaledDestTime = time * (destAnimDuration / sourceAnimDuration);

    sourceAnimClip.setAnimationFrame(mNodes, mAdditiveAnimationMask, time);
    destAnimClip.blendAnimationFrame(mNodes, mAdditiveAnimationMask, scaledDestTime, blendFactor);

    destAnimClip.setAnimationFrame(mNodes, mInvertedAdditiveAnimationMask, scaledDestTime);
    sourceAnimClip.blendAnimationFrame(mNodes, mInvertedAdditiveAnimationMask, time, blendFactor);

    mRootNode->calculateTreeMatrices(mNodeToJoint, mInverseBindMatrices, mJointMatrices);
}

void GltfModel::initializeNodes(RenderData& renderData)
{
    if(mModel)
    {
        std::vector<int> const& nodes = mModel->scenes[0].nodes;

        int const rootNode = nodes.at(0);

        int const nodeCount = mModel->nodes.size();

        renderData.rdNodeCount = nodeCount;

        mNodes.clear();
        mNodes.resize(nodeCount);
        mRootNode =
            GltfNode::createNodeTree(rootNode, *mModel, mNodeToJoint, mInverseBindMatrices, mJointMatrices, mNodes);

        renderData.rdRootNode = mRootNode;
    }
}

void GltfModel::updateAdditiveMask(GltfNode& node, int splitNodeNum)
{
    if(node.mNodeNum == splitNodeNum)
    {
        return;
    }

    mAdditiveAnimationMask[node.mNodeNum] = false;

    for(GltfNodeShared child : node.Children())
    {
        if(child)
        {
            updateAdditiveMask(*child, splitNodeNum);
        }
    }
}

void GltfModel::setSkeletonSplitNode(int nodeNum)
{
    std::ranges::fill(mAdditiveAnimationMask, true);

    if(mRootNode)
    {
        updateAdditiveMask(*mRootNode, nodeNum);
    }

    mInvertedAdditiveAnimationMask = mAdditiveAnimationMask;
    mInvertedAdditiveAnimationMask.flip();
}

void GltfModel::resetAdditiveMasks()
{
    mAdditiveAnimationMask         = std::vector<bool>(mNodes.size(), true);
    mInvertedAdditiveAnimationMask = std::vector<bool>(mNodes.size(), false);
}

void GltfModel::setInverseKinematicsNode(int effectiveNodeNum, int ikChainRootNodeNum)
{
    if(effectiveNodeNum < 0 || effectiveNodeNum >= mNodes.size() || ikChainRootNodeNum < 0 ||
       ikChainRootNodeNum >= mNodes.size())
    {
        return;
    }

    GltfNodeShared currentNode = mNodes[effectiveNodeNum];
    GltfNodeShared rootNode    = mNodes[ikChainRootNodeNum];

    if(currentNode == nullptr || rootNode == nullptr)
    {
        return;
    }

    std::vector<GltfNodeShared> ikNodes = {currentNode};

    while(currentNode != rootNode)
    {
        if(GltfNodeShared const parentNode = currentNode->ParentNode())
        {
            ikNodes.push_back(parentNode);
            currentNode = parentNode;
        }
        else
        {
            break;
        }
    }

    // mIKSolver.setNodes(ikNodes);
}
