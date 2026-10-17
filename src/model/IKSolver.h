#pragma once

#include "GltfNode.h"

class IKSolver
{
public:
    IKSolver(int iterations = 0);

    void setNodes(std::vector<GltfNodeShared> nodes);
    bool solveCCD(glm::vec3 target) const;

private:
    std::vector<GltfNodeShared> mNodes;

    int mIterations  = 0;
    float mThreshold = 0.00001f;
};