#pragma once

#include "GltfNode.h"

#include <memory>

using IKSolverUnique = std::unique_ptr<class IKSolver>;

class IKSolver
{
public:
    IKSolver(std::vector<GltfNodeShared> const& nodes, int iterations = 0, float threshold = 0.00001f);

    virtual ~IKSolver() = default;

    virtual bool solve(glm::vec3 target) = 0;

protected:
    const std::vector<GltfNodeShared> mNodes;

    const int mIterations  = 0;
    const float mThreshold = 0.00001f;
};

class CCDSolver final : public IKSolver
{
public:
    using IKSolver::IKSolver;

    bool solve(glm::vec3 target) override;
};

class FABRIKSolver final : public IKSolver
{
public:
    FABRIKSolver(std::vector<GltfNodeShared> const& nodes, int iterations = 0, float threshold = 0.00001f);

    bool solve(glm::vec3 target) override;

private:
    void solveForward(glm::vec3 target);
    void solveBackward(glm::vec3 target);

    std::vector<float> calculateBoneLengths(std::vector<GltfNodeShared> const& nodes) const;

    void adjustNodes();

    const std::vector<float> mBoneLengths;

    std::vector<glm::vec3> mNodeGlobalPositions;
};
