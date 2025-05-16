#ifndef LIGHT_HPP
#define LIGHT_HPP
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT       1
#define LIGHT_SPOT        2
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



struct Light {
    int type;            // 4 bytes (offset 0)
    glm::vec3 position;  // 12 bytes (offset 16)
    glm::vec3 direction; // 12 bytes (offset 32)
    glm::vec3 ambient;   // 12 bytes (offset 48)
    glm::vec3 diffuse;   // 12 bytes (offset 64)
    glm::vec3 specular;  // 12 bytes (offset 80)
    float constant;      // 4 bytes (offset 92)
    float linear;        // 4 bytes (offset 96)
    float quadratic;     // 4 bytes (offset 100)
    float cutOff;        // 4 bytes (offset 104)
    float outerCutOff;   // 4 bytes (offset 108)
};


#endif