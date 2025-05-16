#pragma once

#include <string>
#include <vector>
#include <memory>

#include <assimp/anim.h>

#include "AssimpAnimChannel.hpp"
#include "Model/AssimpBone.hpp"

class AssimpAnimClip {
  public:
    void addChannels(aiAnimation* animation, std::vector<std::shared_ptr<AssimpBone>> boneList);
    const std::vector<std::shared_ptr<AssimpAnimChannel>>& getChannels();

    std::string getClipName();
    float getClipDuration();
    float getClipTicksPerSecond();

    void setClipName(std::string name);

  private:
    std::string mClipName;
    float mClipDuration = 0.0f;
    float mClipTicksPerSecond = 0.0f;

    std::vector<std::shared_ptr<AssimpAnimChannel>> mAnimChannels{};
};

