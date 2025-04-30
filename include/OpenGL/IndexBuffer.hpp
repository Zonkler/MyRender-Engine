/* Mininmal index buffer class */
#pragma once

#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OGLRenderData.hpp"

class IndexBuffer {
  public:
    void init();
    void uploadData(std::vector<uint32_t> indices);

    void bind();
    void unbind();

    void cleanup();

  private:
    GLuint mIndexVBO = 0;
};
