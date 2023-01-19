#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../include/shader.h"
#include "../src/camera.cpp"

#include "../include/model.h"

#include <iostream>
#include <tuple>



class Renderer
{
public:
    Camera camera;
    GLFWwindow* window;
    Shader ourShader;
    Model ourModel;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    int last_GLFW_MOUSE_BUTTON_LEFT_state = GLFW_RELEASE;
    glm::vec3 selected_vertice;
    int selected_mesh;



    Renderer(string const &model_path, const char* vertexPath, const char* fragmentPath);
    int run();
    void closeRenderer();

private:

    
    int initWindow();

    //input processing
    void processInput();
    void static framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    //vertice selection
    void verticeSelection();
    glm::vec3 Win_to_Obj_Coord(int winX, int winY);
};

