#include "ShadowMapper.hpp"
#include <iostream>

ShadowMapper::ShadowMapper(Type type, unsigned int resolution)
    : m_type(type), m_resolution(resolution), m_fbo(0), m_depthMap(0) {
    
    glGenFramebuffers(1, &m_fbo);
    
    if(m_type == POINT) {
        setupPointLightShadow();
    } else {
        setupDirectionalShadow();
    }
}

ShadowMapper::~ShadowMapper() {
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteTextures(1, &m_depthMap);
}

void ShadowMapper::setupPointLightShadow() {
    // Create depth cubemap texture
    glGenTextures(1, &m_depthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMap);
    
    for(unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F,
                    m_resolution, m_resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Attach depth texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Point light shadow FBO not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapper::setupDirectionalShadow() {
    // Create 2D depth texture
    glGenTextures(1, &m_depthMap);
    glBindTexture(GL_TEXTURE_2D, m_depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 
                m_resolution, m_resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    constexpr float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Directional shadow FBO not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapper::beginRender(const Light& light) {
    glViewport(0, 0, m_resolution, m_resolution);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    if(m_type == POINT) {
        // Calculate shadow matrices for point light
        float aspect = static_cast<float>(m_resolution) / m_resolution;
        float near = 0.1f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, m_farPlane);
        
        m_shadowMatrices = {
            shadowProj * glm::lookAt(light.position, light.position + glm::vec3(1,0,0), glm::vec3(0,-1,0)),
            shadowProj * glm::lookAt(light.position, light.position + glm::vec3(-1,0,0), glm::vec3(0,-1,0)),
            shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0,1,0), glm::vec3(0,0,1)),
            shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0,-1,0), glm::vec3(0,0,-1)),
            shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0,0,1), glm::vec3(0,-1,0)),
            shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0,0,-1), glm::vec3(0,-1,0))
        };
        
        m_lightPos = light.position;
    } else {
        // Calculate orthographic projection for directional light
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
        glm::mat4 lightView = glm::lookAt(light.position, 
                                         light.position + light.direction, 
                                         glm::vec3(0.0f, 1.0f, 0.0f));
        m_shadowMatrices = { lightProjection * lightView };
    }
}

void ShadowMapper::endRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapper::bindTexture(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    if(m_type == POINT) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMap);
    } else {
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
    }
}