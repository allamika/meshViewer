#include <stdio.h>
#include <string>

#include "include/renderer.h"
#include "include/mesh.h"
#include "include/model.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


glm::vec3 selectv;


class MyGui{
public:
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int nb_anneaux = 0, nb_iteration = 0;
    float x=0, y=0, z=0, lambda = 0;
    Renderer *prenderer;


    MyGui(Renderer *renderer){
        prenderer = renderer;
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        const char* glsl_version = "#version 130";

        ImGui_ImplGlfw_InitForOpenGL(renderer->window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);   
    };

    void Frame(){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        static float f = 0.0f;
        static int counter = 0;
        ImGui::Begin("Je suis une fenetre");
        
        ImGui::Text("Metallic :");
        float metallic = prenderer->metallic;
        ImGui::SliderFloat(" ", &metallic, 0.0f, 1.0f);
        prenderer->metallic = metallic;

        ImGui::Text("Roughness :");
        float roughness = prenderer->roughness;
        ImGui::SliderFloat("  ", &roughness, 0.0f, 1.0f);
        prenderer->roughness = roughness;

        ImGui::Text("Ao :");
        float ao = prenderer->ao;
        ImGui::SliderFloat("   ", &ao, 0.0f, 1.0f);
        prenderer->ao = ao;



        ImGui::Text("\nApplication average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    };

    void close(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };
};


// Entry point.
int main(int argc, char** argv)
{
    
    //string model_path = "../scenes/human/human.obj";
    string model_path = "../scenes/cube/cube.obj";
    //string model_path = "../scenes/backpack/backpack.obj";

    
    Renderer renderer(model_path, "../shader/vertex/depthMapping.vs", "../shader/fragment/depthMapping.fs");    
    //Renderer renderer(model_path, "../shader/vertex/camera_normal_texture.vs", "../shader/fragment/lighted_micro_material.fs");
    //Renderer renderer(model_path, "../shader/vertex/camera_normal_texture.vs", "../shader/fragment/lighting_map.fs");
    //Renderer renderer(model_path, "../shader/vertex/camera_normal_texture.vs", "../shader/fragment/lighted_material.fs");
    //Renderer renderer(model_path, "../shader/vertex/camera_normal_texture.vs", "../shader/fragment/texture.fs");
    MyGui gui(&renderer);

    
    
    while(renderer.run()){
        gui.Frame();
        selectv = renderer.selected_vertice;
    }
    
    renderer.closeRenderer();
    return 0;

}
