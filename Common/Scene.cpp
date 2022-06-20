//
// Created by pushpak on 3/28/18.
//

#include "Scene.h"

Scene::Scene() : _windowWidth(100), _windowHeight(100)
{

}

Scene::Scene(int windowWidth, int windowHeight)
{
    _windowHeight = windowHeight;
    _windowWidth = windowWidth;
}

Scene::~Scene()
{
    CleanUp();
}

// Public methods

// Init: called once when the scene is initialized
int Scene::Init()
{
    return -1;
}

// LoadAllShaders: This is the placeholder for loading the shader files
void Scene::LoadAllShaders()
{
    return;
}


// preRender : called to setup stuff prior to rendering the frame
int Scene::preRender()
{
    return -1;
}

// Render : per frame rendering of the scene
int Scene::Render()
{
    return -1;
}

// postRender : Any updates to calculate after current frame
int Scene::postRender()
{
    return -1;
}

// CleanUp : clean up resources before destruction
void Scene::CleanUp()
{
    return;
}

// Display : Per-frame execution of the scene
int Scene::Display()
{
    preRender();

    Render();

    postRender();

    return -1;
}