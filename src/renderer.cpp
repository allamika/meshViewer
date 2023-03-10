#include "../include/renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include "../include/shader.h"
#include "../src/camera.cpp"

#include "../include/model.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include <iostream>

#include <vector>



float xMouse;
float yMouse;

Renderer::Renderer(string const &model_path, const char* vertexPath, const char* fragmentPath){
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    initWindow();
    ourShader = Shader(vertexPath, fragmentPath);
    ourModel = Model(model_path, false);
    //lightShader = Shader(vertexPath, fragmentPath);
    lightShader = Shader("../shader/light/light.vs", "../shader/light/light.fs");
    lightModel = Model("../scenes/cube/cube.obj", false);

    depthShader = Shader("../shader/vertex/depth.vs", "../shader/fragment/depth.fs");

    createScene();

}

void Renderer::createScene(){
    Rendable model1;
    model1.model = ourModel;
    model1.shader = ourShader;
    model1.toWorld = glm::mat4(1.0f);
    models.push_back(model1);

    Rendable model2;
    model2.model = ourModel;
    model2.shader = ourShader;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model2.toWorld = model;
    models.push_back(model2);

    Rendable bigCube;
    bigCube.model = Model("../scenes/cube/cube.obj", false);
    bigCube.shader = ourShader;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));	
    bigCube.toWorld = model;
    models.push_back(bigCube);


    Light lightp1;
    lightp1.type = pointLight;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.0f, 0.0f, 3.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	
    lightp1.toWorld = model;
    lights.push_back(lightp1);

    Light lightp2;
    lightp2.type = pointLight;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.0f, 0.0f, -3.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	
    lightp2.toWorld = model;
    lights.push_back(lightp2);

    Light light2;
    light2.type = directionalLight;
    light2.direction = glm::vec3(-1.0f, -1.0f, 0.0f);
    lights.push_back(light2);   
}

int Renderer::initWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create the window
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MeshViewer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    //tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    //set rendering window size
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    //set callback for window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    //depth map
    const unsigned int SHADOW_WIDTH = 1200, SHADOW_HEIGHT = 800;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    return 0;
}


int Renderer::run(){

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    //process inputs
    processInput();

    glfwSwapBuffers(window);
    glfwPollEvents();glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    
    // render scene from light's point of view
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D,depthMap); 
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    renderdepthModels();



    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderModels();


    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, depthMap);

    renderModels();
    renderLights();

    return !glfwWindowShouldClose(window);
}


void Renderer::renderdepthModels(){
    for(int i=0; i < models.size(); i++){
        std::cout << "shadow" << std::endl;
        Rendable* currentModel = &models[i];
        depthShader.setMat4("model", currentModel->toWorld);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        depthShader.setMat4("lightSpaceMatrix", projection * view);
        currentModel->model.Draw(depthShader);
    }
}
void Renderer::renderModels(){
    // render the loaded model
    for(int i=0; i < models.size(); i++){
        Rendable* currentModel = &models[i];

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        currentModel->shader.setMat4("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        currentModel->shader.setMat4("view", view);
        currentModel->shader.setMat4("model", currentModel->toWorld);
        
        currentModel->shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        currentModel->shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        currentModel->shader.setVec3("material.specular", 0.7f, 0.7f, 0.7f);
        currentModel->shader.setFloat("material.shininess", 32.0f);
        currentModel->shader.setVec3("materialBRDF.albedo", 1.0f, 0.5f, 0.31f);
        currentModel->shader.setFloat("materialBRDF.metallic", metallic);
        currentModel->shader.setFloat("materialBRDF.roughness", roughness);
        currentModel->shader.setFloat("materialBRDF.ao", ao);

        
        currentModel->shader.setVec3("viewPos", camera.Position);
        
        int nbPointLight=0, nbDirLight=0 ;
        for(int i=0; i < lights.size(); i++){
            Light* currentLight = &lights[i];
            switch(currentLight->type){
                case pointLight:{
                    currentModel->shader.setVec3("pointLights[" + std::to_string(nbPointLight) + "].ambient", 0.2f, 0.2f, 0.2f);
                    currentModel->shader.setVec3("pointLights[" + std::to_string(nbPointLight) + "].diffuse", 0.8f, 0.8f, 0.8f); // darken diffuse light a bit
                    currentModel->shader.setVec3("pointLights[" + std::to_string(nbPointLight) + "].specular", 1.0f, 1.0f, 1.0f);

                    glm::vec3 lightPos = currentLight->toWorld * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                    currentModel->shader.setVec3("pointLights[" + std::to_string(nbPointLight) + "].position", lightPos);

                    currentModel->shader.setFloat("pointLights[" + std::to_string(nbPointLight) + "].constant", 1.0f);
                    currentModel->shader.setFloat("pointLights[" + std::to_string(nbPointLight) + "].linear", 0.09f);
                    currentModel->shader.setFloat("pointLights[" + std::to_string(nbPointLight) + "].quadratic", 0.032f);

                    nbPointLight ++;
                }break;  

                case directionalLight:{
                    currentModel->shader.setVec3("directionLights[" + std::to_string(nbDirLight) + "].ambient", 0.2f, 0.2f, 0.2f);
                    currentModel->shader.setVec3("directionLights[" + std::to_string(nbDirLight) + "].diffuse", 0.4f, 0.4f, 0.4f); // darken diffuse light a bit
                    currentModel->shader.setVec3("directionLights[" + std::to_string(nbDirLight) + "].specular", 0.6f, 0.6f, 0.6f);

                    currentModel->shader.setVec3("directionLights[" + std::to_string(nbDirLight) + "].direction", currentLight->direction);

                    nbDirLight ++;
                

                }break;

                default:{
                }break;
            }
        }        

        currentModel->shader.setInt("nbPointLight", nbPointLight);
        currentModel->shader.setInt("nbDirLight", nbDirLight);

        currentModel->model.Draw(currentModel->shader);
    }
}

void Renderer::renderLights(){
    for(int i=0; i < lights.size(); i++){
        Light* currentLight = &lights[i];

        switch(currentLight->type){
            case pointLight:{
                currentLight->shader.use();
                // pass projection matrix to shader (note that in this case it could change every frame)
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                currentLight->shader.setMat4("projection", projection);
                glm::mat4 view = camera.GetViewMatrix();
                currentLight->shader.setMat4("view", view);
                currentLight->shader.setMat4("model", currentLight->toWorld);
                currentLight->model.Draw(currentLight->shader);

            }break;

            case directionalLight:{

                currentLight->shader.use();
                // pass projection matrix to shader (note that in this case it could change every frame)
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                currentLight->shader.setMat4("projection", projection);
                glm::mat4 view = camera.GetViewMatrix();
                currentLight->shader.setMat4("view", view);
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, camera.Position - glm::normalize(currentLight->direction) * 100.0f); 
                currentLight->shader.setMat4("model", model);
                
                currentLight->model.Draw(currentLight->shader);
            }break;

            default:{
            }break;
        }
    }
}


void Renderer::closeRenderer(){
    ourShader.deleteShaderProgram();
    glfwTerminate();
}

//input processing
void Renderer::processInput()
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }


    //mouse input
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
    glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != last_GLFW_MOUSE_BUTTON_LEFT_state){
        verticeSelection();
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != last_GLFW_MOUSE_BUTTON_LEFT_state)
        last_GLFW_MOUSE_BUTTON_LEFT_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    //camera input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(cLEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(cRIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(cUP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(cDOWN, deltaTime);
}



void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}




void Renderer::mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    xMouse = static_cast<float>(xposIn);
    yMouse = static_cast<float>(yposIn);
}


float euclidian_dist(glm::vec3 v1, glm::vec3 v2){
    return sqrt((v1.x-v2.x)*(v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y) + (v1.z-v2.z)*(v1.z-v2.z));
}


void Renderer::verticeSelection(){
    glm::vec3 select = Win_to_Obj_Coord(xMouse, yMouse);

    float currend_dist, min_dist = -1;
    int mesh_min, vertice_min;

    for(int i=0; i<ourModel.meshes.size(); i++){
        for(int j=0; j<ourModel.meshes[i].vertices.size(); j++){
            currend_dist = euclidian_dist(select, ourModel.meshes[i].vertices[j].Position);
            if (currend_dist < min_dist || min_dist == -1){
                min_dist = currend_dist;
                mesh_min = i;
                vertice_min = j;
            }
        }
    }
    selected_mesh = mesh_min;
    selected_vertice = ourModel.meshes[mesh_min].vertices[vertice_min].Position;
}



float near = 0.1f;
float far = 100.0f;

float linearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

glm::vec3 Renderer::Win_to_Obj_Coord(int x, int y)
{
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    //camera transformation: view and 
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    //screen and depth coordonate
    winX = (float)x;
    winY = SCR_HEIGHT - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
    winX = (2.0f * winX)/SCR_WIDTH - 1.0f;
    winY = (2.0f * winY)/SCR_HEIGHT - 1.0f;


    //ray creation
    float rx, ry, rz;
    rx = winX;
    ry = winY;
    rz = 1.0f;
    //clip ray
    glm::vec3 ray_nds = glm::vec3(rx,ry,rz);
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
    //camera ray
    glm::vec4 ray_eye = (inverse(projection)) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
    //world ray
    glm::vec4 ray_wor4 = (inverse(view)) * ray_eye;
    glm::vec3 ray_wor = glm::vec3(ray_wor4.x, ray_wor4.y, ray_wor4.z);
    ray_wor = normalize(ray_wor);

    
    //depth linearization
    winZ = linearizeDepth(winZ);

    //world position
    glm::vec3 pos = camera.Position + ray_wor * winZ;
    return pos;
}



//void Renderer::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
//{
//    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
//        std::cout << "click" << std::endl;
//    }
//        
//}