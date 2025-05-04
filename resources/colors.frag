// Fragment Shader
#version 460 core
layout (location = 0) in vec4 color;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 FragPos;

layout (location = 0) out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 viewPos;

struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

uniform Light lights[4];
uniform int numLights;




vec3 CalculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // Get color from texture only
    vec3 diffuseColor = texture(tex, texCoord).rgb; //* color.rgb;
    
    // Use simple specular calculation
    const float specularStrength = 0.5;
    vec3 specularColor = vec3(specularStrength);

    // Light direction and attenuation
    vec3 lightDir;
    float attenuation = 1.0;
    
    if(light.type == 0) { // Directional
        lightDir = normalize(-light.direction);
    } else {
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + 
                          light.quadratic * (distance * distance));
    }
    
    // Spotlight calculations
    float intensity = 1.0;
    if(light.type == 2) { // Spotlight
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    }
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    // Combine results
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;
    
    return (ambient + (diffuse + specular)) * attenuation * intensity;
}

void main() {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Get base color from texture FIRST
    vec3 texColor = texture(tex, texCoord).rgb;
    
    // Global ambient (not multiplied by texture yet)
    vec3 ambient = vec3(0.1); // Or use a uniform
    
    // Accumulate lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < numLights; i++) {
        lighting += CalculateLight(lights[i], norm, FragPos, viewDir);
    }
    
    // Combine global ambient + per-light contributions
    vec3 result = (ambient * texColor) + lighting;
    
    FragColor = vec4(result, 1.0);
}