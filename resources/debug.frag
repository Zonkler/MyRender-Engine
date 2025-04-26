#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform samplerCube depthMap;
uniform float far_plane;

void main() {
    // Sample from positive X face
    float depth = texture(depthMap, vec3(1.0, 0.0, 0.0)).r;
    // Visualize depth as grayscale
    FragColor = vec4(vec3(depth / far_plane), 1.0);
}