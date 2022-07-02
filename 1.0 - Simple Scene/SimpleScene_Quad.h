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

    std::vector<GameObject*> BVHObjs{ 8 };
    //GameObject* BVHObjs[7];
    bool BVHenabled;
    bool newTree = true;
    BVHierarchy::Node** tree;
    bool renderBV = true;
    int renderDepth = 0; 
    bool bottomUpTree = false;
};


//Externs for BVH
extern bool renderBVHSphere;
extern int indexOfTreeInt;

extern float nearestNeighbourWeight;
extern float combinedVolWeight;
extern float relVolIncreaseWeight;
#endif //SIMPLE_SCENE_SIMPLESCENE_QUAD_H
