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



float xMouse;
float yMouse;

Renderer::Renderer(string const &model_path, const char* vertexPath, const char* fragmentPath){
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    initWindow();
    ourShader = Shader(vertexPath, fragmentPath);
    ourModel = Model(model_path, false);

}

int Renderer::initWindow(){
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //create the window
    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_TRUE);
    //glDepthRange(0, 100);

    //set rendering window size
    glViewport(0, 0, 800, 600);

    //set callback for window resizing
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    
    


    return 0;
}

int Renderer::run(){

    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    //process inputs
    processInput();



    glfwSwapBuffers(window);
    glfwPollEvents();



    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    //glDepthRange(0.0f, 1000.0f);

    


    ourShader.use();

    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);

    glm::mat4 view = camera.GetViewMatrix();
    ourShader.setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    ourShader.setMat4("model", model);
    ourModel.Draw(ourShader);

    


    return !glfwWindowShouldClose(window);
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