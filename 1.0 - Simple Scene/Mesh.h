#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    glm::vec3 Normal;
};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    //vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    Mesh() = default;
    void setupGenericMesh();
    void UpdateGenericMesh();
    void Draw();
    void GenericDraw(); //for non index buffer stuff
    void DrawBoundingVolume();
    void GenericDrawTriangle();
    void CleanUp();

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};