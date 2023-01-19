cmake_minimum_required(VERSION 3.20)
project(meshViewer)

set(CMAKE_PREFIX_PATH "/home/mikael/tools/vcpkg/installed/x64-linux/share")

#GLFW additions
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(assimp)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenMesh CONFIG REQUIRED)


set(INCLUDE 
        "${PROJECT_SOURCE_DIR}/include/shader.h"
        "${PROJECT_SOURCE_DIR}/include/mesh.h"
        "${PROJECT_SOURCE_DIR}/include/model.h"
        "${PROJECT_SOURCE_DIR}/include/renderer.h"
        )
set(SRC 
        "${PROJECT_SOURCE_DIR}/src/shader.cpp"
        "${PROJECT_SOURCE_DIR}/src/camera.cpp"
        "${PROJECT_SOURCE_DIR}/src/mesh.cpp"
        "${PROJECT_SOURCE_DIR}/src/model.cpp"
        "${PROJECT_SOURCE_DIR}/src/renderer.cpp"
        )

set(SOURCE_FILES main.cpp)

add_executable(meshViewer ${SOURCE_FILES} ${SRC} ${INCLUDE})

#add_subdirectory(glfw)
#include_directories(glfw)
#include_directories(glfw/deps)
#include_directories(glfw/include)
#include_directories(libs/assimp/include/)


target_link_libraries(meshViewer 
        PRIVATE glfw 
        PRIVATE glad::glad
        PRIVATE imgui::imgui
        PRIVATE GLEW::GLEW
        assimp
        PRIVATE OpenMeshCore
        PRIVATE OpenMeshTools
        ${CMAKE_DL_LIBS})






#set(GLAD "${GLFW_SOURCE_DIR}/deps/glad/gl.h"
#         "${GLFW_SOURCE_DIR}/deps/glad_gl.c")
#set(GETOPT "${GLFW_SOURCE_DIR}/deps/getopt.h"
#           "${GLFW_SOURCE_DIR}/deps/getopt.c")
#set(TINYCTHREAD "${GLFW_SOURCE_DIR}/deps/tinycthread.h"
#                "${GLFW_SOURTINYCTHREADCE_DIR}/deps/tinycthread.c")
#set(IMGUI "${PROJECT_SOURCE_DIR}/imgui/imgui.h"
#          "${PROJECT_SOURCE_DIR}/imgui/imgui_impl_glfw.h"
#          "${PROJECT_SOURCE_DIR}/imgui/imgui_impl_opengl3.h")




#target_link_libraries(meshViewer glfw assimp ${GLFW_LIBRARIES})

