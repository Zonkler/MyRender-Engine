/* simple OpenGL 4.6 renderer */
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

#include <glm/glm.hpp>

/* GL headers before GLFW */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OGLRenderData.hpp"

#include "Tools/Timer.hpp"
#include "Framebuffer.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "UniformBuffer.hpp"
#include "ShaderStorageBuffer.hpp"
#include "Interface/UserInterface.hpp"
#include "Tools/Camera.hpp"
#include "Model/AssimpModel.hpp"
#include "Model/AssimpInstance.hpp"
#include "Model/ModelAndInstanceData.hpp"

class OGLRenderer {
  public:
    OGLRenderer(GLFWwindow *window);

    bool init(unsigned int width, unsigned int height);
    void setSize(unsigned int width, unsigned int height);

    bool draw(float deltaTime);

    void handleKeyEvents(int key, int scancode, int action, int mods);
    void handleMouseButtonEvents(int button, int action, int mods);
    void handleMousePositionEvents(double xPos, double yPos);

    bool hasModel(std::string modelFileName);
    bool addModel(std::string modelFileName);
    void deleteModel(std::string modelFileName);

    std::shared_ptr<AssimpInstance> addInstance(std::shared_ptr<AssimpModel> model);
    void addInstances(std::shared_ptr<AssimpModel> model, int numInstances);
    void deleteInstance(std::shared_ptr<AssimpInstance> instance);
    void cloneInstance(std::shared_ptr<AssimpInstance> instance);

    void cleanup();

  private:
    OGLRenderData mRenderData{};
    ModelAndInstanceData mModelInstData{};

    Timer mFrameTimer{};
    Timer mMatrixGenerateTimer{};
    Timer mUploadToVBOTimer{};
    Timer mUploadToUBOTimer{};
    Timer mUIGenerateTimer{};
    Timer mUIDrawTimer{};

    Shader mAssimpShader{};
    Shader mAssimpSkinningShader{};

    Framebuffer mFramebuffer{};
    UniformBuffer mUniformBuffer{};
    UserInterface mUserInterface{};
    Camera mCamera{};

    /* for non-animated models */
    std::vector<glm::mat4> mWorldPosMatrices{};
    ShaderStorageBuffer mWorldPosBuffer{};

    /* for animated models */
    std::vector<glm::mat4> mModelBoneMatrices{};
    ShaderStorageBuffer mShaderBoneMatrixBuffer{};

    bool mMouseLock = false;
    int mMouseXPos = 0;
    int mMouseYPos = 0;

    void handleMovementKeys();
    void updateTriangleCount();

    /* create identity matrix by default */
    glm::mat4 mViewMatrix = glm::mat4(1.0f);
    glm::mat4 mProjectionMatrix = glm::mat4(1.0f);

};
