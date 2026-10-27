#pragma once

#include <memory>
#include <vector>

#include "IKSolver.h"
#include "ModelSettings.h"

class GtlfModelShared;
class Mesh;
class GltfModel;
class GltfAnimationClip;

class GltfInstance
{
    GltfInstance(GltfModel const& model, glm::vec3 worldPos, bool const randomize = false);

public:
    void resetNodeData();

    void setSkeletonSplitNode(int nodeNum);

    std::vector<glm::mat4> getJointMatrices() const { return mJointMatrices; }
    glm::quat getWorldRotation() const;

private:
    void updateAdditiveMask(GltfNode& node, int splitNodeNum);
    void resetAdditiveMasks();

    void updateAnimation();

    ModelSettings const& getInstanceSettings() const { return mModelSettings; }

    void checkForUpdates();

    template <typename IKSolverType>
    void setIKNodes(int numIterations, int effectiveNodeNum, int ikChainRootNodeNum)
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

        std::vector<GltfNodeShared> ikNodes = std::vector{currentNode};

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

        mIKSolver = std::make_unique<IKSolverType>(ikNodes, numIterations);
    }

private:
    GltfModel const& mModel;
    ModelSettings mModelSettings;

    std::vector<glm::mat4> mJointMatrices;

    std::vector<bool> mAdditiveAnimationMask;
    std::vector<bool> mInvertedAdditiveAnimationMask;

    GltfNodeShared mRootNode;
    GltfNodesList mNodes;

    std::vector<GltfAnimationClip> mAnimClips;

    Mesh mSkeletonMesh;

    IKSolverUnique mIKSolver;
};