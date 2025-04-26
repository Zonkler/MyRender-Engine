#version 330 core
out vec4 FragColor;

struct Material {
    // Texture properties
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    
    // Color properties
    vec3 diffuseColor;
    vec3 specularColor;
    
    float shininess;
    bool useTextures;
};

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT       1
#define LIGHT_SPOT        2

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

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;

uniform vec3 viewPos;
uniform Material material;
uniform Light lights[4];
uniform int numLights;
uniform float far_plane;
uniform samplerCube depthMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(fs_in.Normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}
float ShadowCalculation_Pointlight(vec3 fragPos, Light light) {
    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight);
    float shadow=0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

vec3 CalculateLight(Light light, Material material, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Determine colors based on texture mode
    vec3 diffuseColor = material.useTextures ? 
        texture(material.texture_diffuse1, fs_in.TexCoords).rgb : 
        material.diffuseColor;
    
    vec3 specularColor = material.useTextures ? 
        texture(material.texture_specular1, fs_in.TexCoords).rgb : 
        material.specularColor;

    vec3 lightDir;
    float attenuation = 1.0;
    
    // Light direction and attenuation
    if(light.type == LIGHT_DIRECTIONAL) {
        lightDir = normalize(-light.direction);
    } else {
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + 
                     light.quadratic * (distance * distance));
    }
    
    // Spotlight intensity
    float intensity = 1.0;
    if(light.type == LIGHT_SPOT) {
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    }
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    // Combine results
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;
    
    float shadow;
    if(light.type == LIGHT_POINT){shadow = ShadowCalculation_Pointlight(fragPos,light); }
    else{ shadow= ShadowCalculation(fs_in.FragPosLightSpace,lightDir);}

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * attenuation * intensity;
}

void main()
{
    // Common calculations
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 result = vec3(0.0);

    // Accumulate light contributions
    for(int i = 0; i < numLights; i++) {
        result += CalculateLight(lights[i], material, norm, fs_in.FragPos, viewDir);
    }

    // Gamma correction
    FragColor = vec4(pow(result, vec3(1.0/2.2)), 1.0);
}