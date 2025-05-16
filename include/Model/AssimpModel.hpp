/* Assimp model, ready to draw */
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glad/glad.h>

#include <glm/glm.hpp>

#include "OpenGL/Texture.hpp"
#include "AssimpMesh.hpp"
#include "AssimpNode.hpp"
#include "AssimpAnimClip.hpp"
#include "OpenGL/VertexIndexBuffer.hpp"
#include "OpenGL/ShaderStorageBuffer.hpp"

#include "OpenGL/OGLRenderData.hpp"

class AssimpModel {
  public:
    bool loadModel(std::string modelFilename, unsigned int extraImportFlags = 0);
    glm::mat4 getRootTranformationMatrix();

    void draw();
    void drawInstanced(int instanceCount);
    unsigned int getTriangleCount();

    std::string getModelFileName();
    std::string getModelFileNamePath();

    bool hasAnimations();
    const std::vector<std::shared_ptr<AssimpAnimClip>>& getAnimClips();

    const std::vector<std::shared_ptr<AssimpNode>>& getNodeList();
    const std::unordered_map<std::string, std::shared_ptr<AssimpNode>>& getNodeMap();

    const std::vector<std::shared_ptr<AssimpBone>>& getBoneList();

    void bindBoneMatrixOffsetBuffer(int bindingPoint);
    void bindBoneParentBuffer(int bindingPoint);

    void cleanup();
private:
    void processNode(std::shared_ptr<AssimpNode> node, aiNode* aNode, const aiScene* scene, std::string assetDirectory);
    void createNodeList(std::shared_ptr<AssimpNode> node, std::shared_ptr<AssimpNode> newNode, std::vector<std::shared_ptr<AssimpNode>> &list);

    unsigned int mTriangleCount = 0;
    unsigned int mVertexCount = 0;

    /* store the root node for direct access */
    std::shared_ptr<AssimpNode> mRootNode = nullptr;
    /* a map to find the node by name */
    std::unordered_map  <std::string, std::shared_ptr<AssimpNode>> mNodeMap{};
    /* and a 'flat' map to keep the order of insertation  */
    std::vector<std::shared_ptr<AssimpNode>> mNodeList{};

    std::vector<std::shared_ptr<AssimpBone>> mBoneList{};

    std::vector<std::shared_ptr<AssimpAnimClip>> mAnimClips{};

    std::vector<OGLMesh> mModelMeshes{};
    std::vector<VertexIndexBuffer> mVertexBuffers{};

    ShaderStorageBuffer mShaderBoneParentBuffer{};
    ShaderStorageBuffer mShaderBoneMatrixOffsetBuffer{};

    // map textures to external or internal texture names
    std::unordered_map<std::string, std::shared_ptr<Texture>> mTextures{};
    std::shared_ptr<Texture> mPlaceholderTexture = nullptr;
    std::shared_ptr<Texture> mWhiteTexture = nullptr;

    glm::mat4 mRootTransformMatrix = glm::mat4(1.0f);

    std::string mModelFilenamePath;
    std::string mModelFilename;
};

