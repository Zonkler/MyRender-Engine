/*--------------- Third party  ---------------*/
//-- Glad / GLFW --
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//-- GLM for linear algebra --
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//-- Imgui --
#include <imgui.h>
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

//-- Assimp --
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//-- STB --
#include "stb_image/stb_image.h"

/*--------------- My classes/files ---------------*/
#include <camera.hpp>
#include <model.hpp>
#include <shadowmapper.hpp>
#include "LoadShaders.hpp"

/*--------------- Standard library ---------------*/
#include <iostream>
#include <memory> // for smart pointers


bool unlocked_cursor=false;

Light greenPointLight{
    .type = LIGHT_POINT,
    .position = glm::vec3(1.85724f, 0.875508f, 1.70584f),
    // Darker green ambient (10% of diffuse)
    .ambient = glm::vec3(0.1f, 0.07f, 0.02f),  
    // Vibrant green (RGB: 30% red, 100% green, 30% blue)
    .diffuse = glm::vec3(1.0f, 0.9f, 0.1f),    
    // Brighter green for specular highlights
    .specular = glm::vec3(1.2f, 1.0f, 0.3f),   
    .constant = 1.0f,
    .linear = 0.09f,  // Slightly less attenuation
    .quadratic = 0.032f
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




int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8); // 4x MSAA
    
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

    /*-------- Load openGL function pointers --------*/
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /*-------- Configure OpenGL global state --------*/
    glEnable(GL_DEPTH_TEST);
    stbi_set_flip_vertically_on_load(true);

    /*-------- Build and compile the shaders --------*/
    Shader lightingShader("../resources/colors.vert", "../resources/colors.frag");
    Shader simpleDepthShader("../resources/model.vert","../resources/model.frag");
    Shader pointshadowshader("../resources/shadow_mappin/depth_shaders/depth_shader.vert","../resources/shadow_mappin/depth_shaders/depth_shader.frag","../resources/shadow_mappin/depth_shaders/depth_shader.geom");

    /*-------- Create Shadow mapping objects --------*/
    ShadowMapper pointLightShadow(ShadowMapper::POINT, 2048);
    pointLightShadow.setFarPlane(1000.0f);

    ShadowMapper dirShadow(ShadowMapper::DIRECTIONAL, 4096);
    dirShadow.setFarPlane(50.5f);

    /*-------- Load custom models --------*/
    Model ourModel("../assets/teapot/teapot.obj");
    Model plane("../assets/woodplane/woodplane.obj");

    /*-------- Enable IMGUI --------*/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    glEnable(GL_MULTISAMPLE); // Enable MSAA globally

    /*-------- Main render loop --------*/
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
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();







        // Point light shadow pass
        pointLightShadow.beginRender(greenPointLight);
        pointshadowshader.use();
        pointshadowshader.setVec3("lightPos", greenPointLight.position);
        pointshadowshader.setFloat("far_plane", pointLightShadow.m_farPlane);
        for (int i = 0; i < 6; ++i) {
            pointshadowshader.setMat4("shadowMatrices[" + std::to_string(i) + "]",pointLightShadow.getShadowMatrices()[i]);
        }
        RenderScene(pointshadowshader, ourModel, plane);
        pointLightShadow.endRender();


        // Main render pass
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        lightingShader.use();
        pointLightShadow.bindTexture(GL_TEXTURE3);
            
        
        lightingShader.setFloat("far_plane", pointLightShadow.m_farPlane);

        

        lightingShader.setInt("shadowMap", 2);

        // Set common uniforms
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 64.0f);

        // Set up lights
        lightingShader.setInt("numLights", 1); // Using 2 lights: directional + spotlight

        lightingShader.setInt("material.texture_diffuse1", 0);
        lightingShader.setInt("material.texture_specular1", 1);
        lightingShader.setInt("shadowMap", 2);
        

        lightingShader.setLight("lights[0]",greenPointLight);
        // View/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        lightingShader.setInt("depthMap",3);
        RenderScene(lightingShader,ourModel,plane);




        ImGui::Begin("Render engine config");
        ImGui::Text("hi");
        ImGui::SliderFloat("Light X position",&greenPointLight.position.x,-20.0f,20.0f);
        ImGui::SliderFloat("Light Y position",&greenPointLight.position.y,-20.0f,20.0f);
        ImGui::SliderFloat("Light Z position",&greenPointLight.position.z,-20.0f,20.0f);

        ImGui::End();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
    if (glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        unlocked_cursor = !unlocked_cursor;  // Flip the boolean

        if (unlocked_cursor)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else{glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);}
        
        

    }
    
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
    if (unlocked_cursor) return; // Skip processing if cursor is unlocked

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


void RenderScene(Shader& shader, Model& teapot, Model& floor)
{
    // Teapot (should use textures)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f));
    shader.setMat4("model", model);
    shader.setBool("material.useTextures", true);  // Critical fix
    teapot.Draw(shader);

            // Teapot (should use textures)
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-6.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.4f));
            shader.setMat4("model", model);
            shader.setBool("material.useTextures", true);  // Critical fix
            teapot.Draw(shader);

    // Floor (using colored material)
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(10.0f));
    shader.setMat4("model", model);
    shader.setBool("material.useTextures", false); // Use color properties
    shader.setVec3("material.diffuseColor", 0.8f, 0.8f, 0.8f);
    shader.setVec3("material.specularColor", 1.0f, 1.0f, 1.0f);
    shader.setFloat("material.shininess", 64.0f);
    floor.Draw(shader);
}

