//
// Created by pushpak on 6/1/18.
//

#ifndef SIMPLE_SCENE_SIMPLESCENE_QUAD_H
#define SIMPLE_SCENE_SIMPLESCENE_QUAD_H

#include "../Common/Scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <vector>
#include "GameObject.h"
#include "Model.h"
#include "Camera.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "BVHierarchy.h"
//#include "Collision.h"


class SimpleScene_Quad : public Scene
{

public:
    SimpleScene_Quad() = default;
    SimpleScene_Quad( int windowWidth, int windowHeight );
    virtual ~SimpleScene_Quad();


public:
    int Init() override;
    void CleanUp() override;

    void CollisionCheck(GameObject& first, GameObject& second);
    int preRender() override;
    int Render() override;
    int postRender() override;
    inline GLuint GetProgramID() { return programID; }
    inline Camera* GetCamera() { return &camera; }
    void RenderTree(BVHierarchy::Node** tree, const glm::mat4& projection, const glm::mat4& view);
    void FreeTree(BVHierarchy::Node* node);
private:

    // member functions
    void initMembers();

    void SetupNanoGUI(GLFWwindow *pWwindow) override;

    // data members
    GLuint  programID;
    GLfloat   angleOfRotation;
    Model ourModel{}; //To be removed
    Camera camera{};
    Camera miniMapCam{};
    std::map<const char*, Model> models; //contain all the models and their ids (indices)
    std::map<int, const char*> intModelID; //contain all the models and their ids (indices) in int format
    std::vector<GameObject> gameObjList;
    std::map<const char*, Collision::Collider*> boundingVolume;
    bool show_demo_window = false;
    bool show_control_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::vector<GameObject*> BVHObjs{ 7 };
    //GameObject* BVHObjs[7];
    bool BVHenabled;
    bool newTree = true;
    BVHierarchy::Node** tree;
    bool renderBV = true, runOnce = false;
    int renderDepth = 0; 
};


#endif //SIMPLE_SCENE_SIMPLESCENE_QUAD_H