#include "OpenGL/UniformBuffer.hpp"
#include "Tools/Logger.hpp"

void UniformBuffer::init(size_t bufferSize) {
  mBufferSize = bufferSize;

  glGenBuffers(1, &mUboBuffer);

  glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);
  glBufferData(GL_UNIFORM_BUFFER, mBufferSize, nullptr, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::uploadUboData(std::vector<glm::mat4> bufferData, int bindingPoint) {
  if (bufferData.empty()) {
    return;
  }
  size_t bufferSize = bufferData.size() * sizeof(glm::mat4);
  glBindBuffer(GL_UNIFORM_BUFFER, mUboBuffer);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, bufferData.data());
  glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, mUboBuffer, 0, bufferSize);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::cleanup() {
  glDeleteBuffers(1, &mUboBuffer);
}
