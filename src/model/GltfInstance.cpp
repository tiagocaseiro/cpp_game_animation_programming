#include "GltfInstance.h"

#include <algorithm>

#include "GltfModel.h"
#include "opengl/RenderData.h"

GltfInstance::GltfInstance(GltfModel const& model, glm::vec3 worldPos, bool const randomize) : mModel(model)
{

    mModelSettings.msWorldPosition = worldPos;

    resetAdditiveMasks();

    mModel.initializeNodes(mRootNode, mNodes, mJointMatrices);

    if(mRootNode)
    {
        mRootNode->setWorldPosition(mModelSettings.msWorldPosition);
    }

    int const nodeCount = mNodes.size();

    mModelSettings.msSkelSplitNode = nodeCount - 1;

    for(GltfNodeShared node : mNodes)
    {
        if(node)
        {
            mModelSettings.msSkelSplitNodeNames.push_back(node->mNodeName.c_str());
        }
        else
        {
            mModelSettings.msSkelSplitNodeNames.push_back("(invalid)");
        }
    }

    model.initializeAnimationClips(mAnimClips);

    for(GltfAnimationClip const& animClip : mAnimClips)
    {
        mModelSettings.msAnimationClipNames.push_back(animClip.mName.c_str());
    }

    if(randomize)
    {
        int const animClipSize = mAnimClips.size();
        int const animClip     = std::rand() % animClipSize;

        float const animSpeed    = (std::rand() % 100) / 100.0f + 0.5;
        float const initRotation = std::rand() % 360 - 180;

        mModelSettings.msAnimClip      = animClip;
        mModelSettings.msAnimSpeed     = animSpeed;
        mModelSettings.msWorldPosition = glm::vec3(0.0f, initRotation, 0.0f);
    }

    checkForUpdates();

    mSkeletonMesh.vertices.resize(nodeCount);

    mModelSettings.msIKEffectorNode = 19;
    mModelSettings.msIKRootNode     = 26;

    mModelSettings.msIkTargetWorldPos = getWorldRotation() * mModelSettings.msIKTargetPosition + worldPos;
}

void GltfInstance::updateAdditiveMask(GltfNode& node, int splitNodeNum)
{
    if(node.mNodeNum == splitNodeNum)
    {
        return;
    }

    mAdditiveAnimationMask[node.mNodeNum] = false;

    for(GltfNodeShared child : node.children())
    {
        if(child)
        {
            updateAdditiveMask(*child, splitNodeNum);
        }
    }
}

glm::quat GltfInstance::getWorldRotation() const
{
    glm::vec3 const to_radians =
        glm::vec3(glm::radians(mModelSettings.msWorldRotation.x), glm::radians(mModelSettings.msWorldRotation.y),
                  glm::radians(mModelSettings.msWorldRotation.z));

    return glm::normalize(glm::quat(to_radians));
}

void GltfInstance::setSkeletonSplitNode(int nodeNum)
{
    std::ranges::fill(mAdditiveAnimationMask, true);

    if(mRootNode)
    {
        updateAdditiveMask(*mRootNode, nodeNum);
    }

    mInvertedAdditiveAnimationMask = mAdditiveAnimationMask;
    mInvertedAdditiveAnimationMask.flip();
}

void GltfInstance::resetAdditiveMasks()
{
    int const nodeCount = mNodes.size();

    mAdditiveAnimationMask         = std::vector<bool>(nodeCount, true);
    mInvertedAdditiveAnimationMask = std::vector<bool>(nodeCount, false);
}

void GltfInstance::checkForUpdates() {}
