#ifndef SHADOWMAPPER_HPP
#define SHADOWMAPPER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "light.hpp"
#include "vector"

class ShadowMapper {
public:
    enum Type { POINT, DIRECTIONAL };
    
    ShadowMapper(Type type, unsigned int resolution);
    ~ShadowMapper();
    
    void beginRender(const Light& light);
    void endRender();
    void bindTexture(GLenum textureUnit) const;
    void setFarPlane(float farPlane) { m_farPlane = farPlane; }

    unsigned int getResolution() const { return m_resolution; }
    const glm::mat4* getShadowMatrices() const { return m_shadowMatrices.data(); }

private:
    Type m_type;
    unsigned int m_resolution;
    GLuint m_fbo;
    GLuint m_depthMap;
    std::vector<glm::mat4> m_shadowMatrices;
    glm::vec3 m_lightPos;
    public:
    float m_farPlane = 100.0f; // Default value

    void setupPointLightShadow();
    void setupDirectionalShadow();
};

#endif