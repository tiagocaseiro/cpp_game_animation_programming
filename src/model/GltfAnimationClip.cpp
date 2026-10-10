#include "GltfAnimationClip.h"

#include <tiny_gltf.h>

GltfAnimationClip GltfAnimationClip::make(tinygltf::Model const& model, const tinygltf::Animation& animation)
{
    std::vector<GltfAnimationChannelShared> channels;
    channels.reserve(animation.channels.size());
    for(auto const& animationChannel : animation.channels)
    {
        channels.push_back(GltfAnimationChannel::make(model, animation, animationChannel));
    }

    return GltfAnimationClip(animation.name, channels);
}

GltfAnimationClip::GltfAnimationClip(std::string const& name, std::vector<GltfAnimationChannelShared> const& channels)
    : mName(name), mChannels(channels)
{
}

void GltfAnimationClip::setAnimationFrame(std::vector<GltfNodeShared> const& nodes,
                                          std::vector<bool> const& additiveAnimationMask, float time) const
{
    for(GltfAnimationChannelShared const& channel : mChannels)
    {
        if(channel == nullptr)
        {
            continue;
        }

        int const targetNodeIndex = channel->mTargetNode;

        if(targetNodeIndex >= nodes.size() || targetNodeIndex >= additiveAnimationMask.size() ||
           additiveAnimationMask[targetNodeIndex] == false)
        {
            continue;
        }

        GltfNodeShared targetNode = nodes[targetNodeIndex];

        if(targetNode == nullptr)
        {
            continue;
        }

        switch(channel->mTargetPath)
        {
            case ETargetPath::ROTATION:
            {
                glm::quat rotation = channel->getRotation(time);
                targetNode->setRotation(rotation);
                break;
            }
            case ETargetPath::TRANSLATION:
            {
                glm::vec3 translation = channel->getTranslation(time);
                targetNode->setTranslation(translation);
                break;
            }
            case ETargetPath::SCALE:
            {
                glm::vec3 scale = channel->getScaling(time);
                targetNode->setScale(scale);
                break;
            }

            default:
                break;
        }
    }

    for(GltfNodeShared const& node : nodes)
    {
        if(node)
        {
            node->calculateLocalTransform();
        }
    }
}

void GltfAnimationClip::blendAnimationFrame(std::vector<GltfNodeShared> const& nodes,
                                            std::vector<bool> const& additiveAnimationMask, float time,
                                            float blendFactor) const
{
    for(GltfAnimationChannelShared const& channel : mChannels)
    {
        if(channel == nullptr)
        {
            continue;
        }

        int const targetNodeIndex = channel->mTargetNode;

        if(targetNodeIndex >= nodes.size())
        {
            continue;
        }

        GltfNodeShared targetNode = nodes[targetNodeIndex];

        if(targetNode == nullptr)
        {
            continue;
        }

        if(targetNodeIndex >= additiveAnimationMask.size() || additiveAnimationMask[targetNodeIndex] == false)
        {
            continue;
        }

        switch(channel->mTargetPath)
        {
            case ETargetPath::ROTATION:
            {
                glm::quat rotation = channel->getRotation(time);
                targetNode->blendRotation(rotation, blendFactor);
                break;
            }
            case ETargetPath::TRANSLATION:
            {
                glm::vec3 translation = channel->getTranslation(time);
                targetNode->blendTranslation(translation, blendFactor);
                break;
            }
            case ETargetPath::SCALE:
            {
                glm::vec3 scale = channel->getScaling(time);
                targetNode->blendScale(scale, blendFactor);
                break;
            }

            default:
                break;
        }
    }

    for(GltfNodeShared const& node : nodes)
    {
        if(node)
        {
            node->calculateLocalTransform();
        }
    }
}

float GltfAnimationClip::getClipEndTime() const
{
    if(mChannels.empty())
    {
        return 0.0f;
    }
    return mChannels.front()->getMaxTime();
}
