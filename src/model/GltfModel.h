#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "GltfAnimationClip.h"
#include "GltfCommon.h"
#include "opengl/Texture.h"
#include "tools/Macros.h"

SHARED_ONLY(GltfModel)

class OGLRenderData;

namespace tinygltf
{
    class Model;
}

class GltfModel
{
public:
    static GltfModelShared make(OGLRenderData& renderData, const std::string& modelFilename,
                                const std::string& textureFilename);

    ~GltfModel();

    void draw();

    void uploadVertexBuffers();
    void uploadIndexBuffer();

    void playAnimation(int animNum, float speedDivider, float blendFactor);
    void playAnimation(int sourceAnimNum, int destAnimNum, float speedDivider, float blendFactor);

    std::string getClipName(int animNum) const;
    float getClipEndTime(int animNum) const;

    const std::vector<glm::mat4>& getJointMatrices() const { return mJointMatrices; }
    // const std::vector<glm::mat2x4>& getJointDualQuats() const { return mJointDualQuats; }

    void blendAnimationFrame(int animNum, float time, float blendFactor);
    void crossBlendAnimationFrame(int sourceAnimNum, int destAnimNum, float time, float blendFactor);

    void initializeNodes(OGLRenderData& renderData);

    void updateAdditiveMask(GltfNode& node, int splitNodeNum);

    void setSkeletonSplitNode(int nodeNum);

    void resetAdditiveMasks();

    void setInverseKinematicsNode(int effectiveNodeNum, int ikChainRootNodeNum);

    std::vector<GltfNodeShared> const nodes() const { return mNodes; }

private:
    GltfModel(const std::shared_ptr<tinygltf::Model>& model, const std::shared_ptr<Texture>& tex,
              OGLRenderData& renderData);

    void createVertexBuffers();
    void createIndexBuffer();

    int getTriangleCount() const;

    std::shared_ptr<tinygltf::Model> mModel;
    GltfNodeShared mRootNode;

    std::vector<GltfNodeShared> mNodes;

    std::vector<int> mNodeToJoint;

    // std::vector<glm::mat2x4> mJointDualQuats;

    std::vector<glm::mat4> mInverseBindMatrices;
    std::vector<glm::mat4> mJointMatrices;

    std::vector<GLuint> mVertexVBO;

    std::vector<GltfAnimationClip> mAnimClips;

    std::vector<bool> mAdditiveAnimationMask;
    std::vector<bool> mInvertedAdditiveAnimationMask;

    TextureShared mTex;

    GLuint mVAO      = 0;
    GLuint mIndexVBO = 0;
};