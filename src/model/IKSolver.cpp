#include "IKSolver.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/quaternion.hpp>

IKSolver::IKSolver(std::vector<GltfNodeShared> const& nodes, int iterations, float threshold)
    : mNodes(nodes), mIterations(iterations), mThreshold(threshold)
{
}

bool CCDSolver::solve(glm::vec3 target)
{
    if(mNodes.empty() || mNodes.front() == nullptr)
    {
        return false;
    }

    GltfNodeShared const effector = mNodes.front();

    auto const isFinished = [effector, target, this] {
        glm::vec3 const effectorGlobalPosition = effector->globalPosition();
        return glm::distance(target, effectorGlobalPosition) <= mThreshold;
    };

    for(int i = 0; i != mIterations; i++)
    {
        for(int j = 1; j < mNodes.size(); j++)
        {
            if(isFinished())
            {
                return true;
            }

            GltfNodeShared const node = mNodes[j];

            if(node == nullptr)
            {
                continue;
            }

            glm::vec3 const nodeGlobalPosition = node->globalPosition();
            glm::quat const nodeGlobalRotation = node->globalRotation();

            glm::vec3 const effectorGlobalPosition = effector->globalPosition();

            glm::vec3 const toEffector = glm::normalize(effectorGlobalPosition - nodeGlobalPosition);
            glm::vec3 const toTarget   = glm::normalize(target - nodeGlobalPosition);

            glm::quat const effectorToTargetGlobalRotation = glm::rotation(toEffector, toTarget);

            glm::quat const effectorToTargetLocalRotation =
                nodeGlobalRotation * effectorToTargetGlobalRotation * glm::conjugate(nodeGlobalRotation);

            glm::quat const nodeLocalRotation = node->localRotation();

            node->blendRotation(nodeLocalRotation * effectorToTargetLocalRotation, 1.0f);

            node->calculateNodeMatrices();
        }
    }

    return false;
}

FABRIKSolver::FABRIKSolver(std::vector<GltfNodeShared> const& nodes, int iterations, float threshold)
    : IKSolver(nodes, iterations, threshold), mNodeGlobalPositions(mNodes.size())
{
}

bool FABRIKSolver::solve(glm::vec3 target)
{
    if(mNodes.empty() || mNodes.front() == nullptr)
    {
        return false;
    }

    mBoneLengths = calculateBoneLengths(mNodes);

    GltfNodeShared const effector = mNodes.front();

    auto const isFinished = [effector, target, this] {
        glm::vec3 const effectorGlobalPosition = effector->globalPosition();
        return glm::distance(target, effectorGlobalPosition) <= mThreshold;
    };

    for(int i = 0; i != mNodes.size(); i++)
    {
        if(GltfNodeShared node = mNodes[i])
        {
            mNodeGlobalPositions[i] = node->globalPosition();
        }
    }

    glm::vec3 base;

    if(GltfNodeShared const baseNode = mNodes.back())
    {
        base = baseNode->globalPosition();
    }

    for(int i = 0; i != mIterations; i++)
    {
        for(int j = 1; j < mNodes.size(); j++)
        {
            if(isFinished())
            {
                adjustNodes();
                return true;
            }
            solveForward(target);
            solveBackward(base);
            adjustNodes();

            if(isFinished())
            {
                return true;
            }
        }
    }

    return false;
}

void FABRIKSolver::solveForward(glm::vec3 target)
{
    mNodeGlobalPositions.front() = target;

    for(int i = 1; i != mNodeGlobalPositions.size(); i++)
    {
        glm::vec3 const direction = glm::normalize(mNodeGlobalPositions[i] - mNodeGlobalPositions[i - 1]);
        glm::vec3 const offset    = direction * mBoneLengths[i - 1];

        mNodeGlobalPositions[i] = mNodeGlobalPositions[i - 1] + offset;
    }
}

void FABRIKSolver::solveBackward(glm::vec3 base)
{
    mNodeGlobalPositions.back() = base;

    for(int i = mNodeGlobalPositions.size() - 2; i >= 0; i--)
    {
        glm::vec3 const direction = glm::normalize(mNodeGlobalPositions[i] - mNodeGlobalPositions[i + 1]);
        glm::vec3 const offset    = direction * mBoneLengths[i];

        mNodeGlobalPositions[i] = mNodeGlobalPositions[i + 1] + offset;
    }
}

std::vector<float> FABRIKSolver::calculateBoneLengths(std::vector<GltfNodeShared> const& nodes) const
{
    std::vector<float> boneLengths;
    if(nodes.empty() == false)
    {
        boneLengths.resize(nodes.size() - 1);
    }

    for(int i = 0; i != nodes.size() - 1; i++)
    {
        GltfNodeShared const node1 = nodes[i];
        GltfNodeShared const node2 = nodes[i + 1];

        if(node1 && node2)
        {
            float const boneLength = glm::distance(node1->globalPosition(), node2->globalPosition());

            boneLengths[i] = boneLength;
        }
    }
    return boneLengths;
}

void FABRIKSolver::adjustNodes()
{
    for(int i = mNodeGlobalPositions.size() - 1; i > 0; i--)
    {
        GltfNodeShared currentNode = mNodes[i];
        GltfNodeShared nextNode    = mNodes[i - 1];

        glm::vec3 const currentNodeGlobalPosition = currentNode->globalPosition();
        glm::quat const currentNodeGlobalRotation = currentNode->globalRotation();

        glm::vec3 const nextNodeGlobalPosition = nextNode->globalPosition();

        glm::vec3 const toNext    = glm::normalize(nextNodeGlobalPosition - currentNodeGlobalPosition);
        glm::vec3 const toDesired = glm::normalize(mNodeGlobalPositions[i - 1] - currentNodeGlobalPosition);

        glm::quat const nodeRotation = glm::rotation(toNext, toDesired);
        glm::quat const localRotation =
            currentNodeGlobalRotation * nodeRotation * glm::conjugate(currentNodeGlobalRotation);

        glm::quat const currentNodeLocalRotation = currentNode->localRotation();

        currentNode->blendRotation(currentNodeLocalRotation * localRotation, 1.0f);

        currentNode->calculateNodeMatrices();
    }
}
