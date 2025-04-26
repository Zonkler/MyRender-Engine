#ifndef LOAD_SHADERS_HPP
#define LOAD_SHADERS_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <light.hpp>
#include <string>

/*----------------------------------------Shader class----------------------------------------*/
// The shader class provides a way to link and compile all the shaders into one shader 
// program that can later be used by the application. It provides 2 constructors, one that takes a path to the 
// vertex shader and the fragment shader and one that takes a vertex, fragment and geometry shader. 
// It also provides error diagnostics and comes along with setter functions.

struct Light; // forward declare

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath);

    Shader(const char* vertexPath, const char* fragmentPath,const char* geometrypath);

    // ------------------------------------------------------------------------
    void use() const;
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const;
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const;
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const;
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    void setLight(const std::string& name, const Light& light);

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type);
};
#endif