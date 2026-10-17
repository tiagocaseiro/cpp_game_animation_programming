#include "IKSolver.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/quaternion.hpp>

IKSolver::IKSolver(int iterations) : mIterations(iterations) {}

void IKSolver::setNodes(std::vector<GltfNodeShared> nodes) { mNodes = nodes; }

bool IKSolver::solveCCD(glm::vec3 target) const
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
            if(isFinished())
            {
                return true;
            }
        }
    }

    return false;
}
