#pragma once

#include "GltfNode.h"

class IKSolver
{
public:
    IKSolver(int iterations = 0);

    void setNodes(std::vector<GltfNodeShared> nodes);
    bool solveCCD(glm::vec3 target);
    bool solveFABRIK(glm::vec3 target);

private:
    void solveFABRIKForward(glm::vec3 target);
    void solveFABRIKBackward(glm::vec3 target);

    void calculateBoneLengths();
    void adjustFABRIKNodes();

    std::vector<float> mBoneLengths;
    std::vector<glm::vec3> mFABRIKNodePositions;

    std::vector<GltfNodeShared> mNodes;

    int mIterations  = 0;
    float mThreshold = 0.00001f;
};