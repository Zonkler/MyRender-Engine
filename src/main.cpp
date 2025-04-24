#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include<assimp/postprocess.h>
#include <memory>

#include "LoadShaders.cpp"
#include <camera.hpp>
#include <model.hpp>
#include <iostream>

Light purpleLight{
    .type = LIGHT_POINT,
    .position = glm::vec3(-5.70244f, 4.13403f, 6.41498f),
    // More subtle ambient (10% of diffuse)
    .ambient = glm::vec3(0.066f, 0.019f, 0.075f),  
    // Rich purple (RGB: 66% red, 19% green, 75% blue)
    .diffuse = glm::vec3(0.66f, 0.19f, 0.75f),    
    // Slightly brighter purple for specular
    .specular = glm::vec3(0.8f, 0.3f, 0.9f),      
    .constant = 1.0f,
    .linear = 0.09f,
    .quadratic = 0.032f
};

Light greenPointLight{
    .type = LIGHT_POINT,
    .position = glm::vec3(1.85724f, 0.875508f, 1.70584f),
    // Darker green ambient (10% of diffuse)
    .ambient = glm::vec3(0.03f, 0.1f, 0.03f),  
    // Vibrant green (RGB: 30% red, 100% green, 30% blue)
    .diffuse = glm::vec3(0.3f, 1.0f, 0.3f),    
    // Brighter green for specular highlights
    .specular = glm::vec3(0.5f, 1.2f, 0.5f),   
    .constant = 1.0f,
    .linear = 0.07f,  // Slightly less attenuation
    .quadratic = 0.017f
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void RenderScene(Shader& shader, Model& teapot,Model& floor);
// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 3.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


void renderQuad()
{
        GLuint quadVAO,quadVBO;
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    stbi_set_flip_vertically_on_load(true);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("../resources/colors.vert", "../resources/colors.frag");
    Shader simpleDepthShader("../resources/model.vert","../resources/model.frag");
    Shader debugDepthQuad("../resources/debug.vert", "../resources/debug.frag");
    //Shader lightingShader("../resources/model.vert", "../resources/model.frag");

    //Shader lightCubeShader("../resources/light_cube.vert", "../resources/light_cube.frag");


    //shaders
    Model ourModel("../assets/teapot/teapot.obj");
    Model plane("../assets/woodplane/woodplane.obj");

    //Model ourModel("../assets/teapot/teapot.obj");
    //glEnable(GL_FRAMEBUFFER_SRGB);


    //creating the framebuffer object

    unsigned int depthMapFBO;
    glGenFramebuffers(1,&depthMapFBO);

    //create the 2D texture 
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    
    unsigned int depthMap;
    glGenTextures(1,&depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER,depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // shader configuration
    // --------------------
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float near_plane = 0.2f, far_plane = 10.5f;
        glm::mat4 lightProjection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,near_plane,far_plane);
        glm::mat4 lightView = glm::lookAt(greenPointLight.position,
                                            glm::vec3( 0.0f, 0.0f, 0.0f),
                                            glm::vec3( 0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix",lightSpaceMatrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        RenderScene(simpleDepthShader,ourModel,plane);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // render Depth map to quad for visual debugging
        // ---------------------------------------------
        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderQuad();



        








        // In your render loop, before drawing:
        lightingShader.use();

        // Set common uniforms
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 64.0f);

        // Set up lights
        lightingShader.setInt("numLights", 2); // Using 2 lights: directional + spotlight


        // Player spotlight (red)
        static Light playerSpotlight;
        playerSpotlight.type = LIGHT_SPOT;
        playerSpotlight.position = camera.Position;
        playerSpotlight.direction = camera.Front;
        playerSpotlight.ambient = glm::vec3(0.1f, 0.0f, 0.0f);
        playerSpotlight.diffuse = glm::vec3(1.0f);
        playerSpotlight.specular = glm::vec3(1.0f);
        playerSpotlight.constant = 1.0f;
        playerSpotlight.linear = 0.09f;
        playerSpotlight.quadratic = 0.032f;
        playerSpotlight.cutOff = glm::cos(glm::radians(12.5f));
        playerSpotlight.outerCutOff = glm::cos(glm::radians(17.5f));
        lightingShader.setLight("lights[0]", purpleLight);
        lightingShader.setLight("lights[1]",greenPointLight);
        // View/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);


        //std::cout<<camera.Positio n.x<<" "<<camera.Position.y<<" "<<camera.Position.z<<'\n';

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
//-5.70244 4.13403 6.41498
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void RenderScene(Shader& shader, Model& teapot,Model& floor)
{        // Draw model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f));
    shader.setMat4("model", model);
    // For non-textured models (like teapot):
    shader.setVec3("material.diffuseColor", 0.8f, 0.8f, 0.8f);  // Solid white
    shader.setVec3("material.specularColor", 1.0f, 1.0f, 1.0f); // Bright highlights
    shader.setFloat("material.shininess", 64.0f);
    teapot.Draw(shader);

    model = glm::scale(model, glm::vec3(4.0f));
    shader.setMat4("model", model);
    floor.Draw(shader);


}
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}