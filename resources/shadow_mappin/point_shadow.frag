#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;  // Added missing uniform
uniform samplerCube depthMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;
uniform bool useTextures;  // Added material control

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = max(0.05 * (1.0 - dot(normalize(fs_in.Normal), normalize(fragToLight))), 0.005);
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

void main()
{
    vec3 color = useTextures ? 
        texture(diffuseTexture, fs_in.TexCoords).rgb : 
        vec3(0.8);  // Default color if no texture

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    
    // Ambient
    vec3 ambient = 0.3 * color;
    
    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = spec * vec3(0.5);
    
    // Shadow
    float shadow = ShadowCalculation(fs_in.FragPos);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    
    FragColor = vec4(lighting, 1.0);
}