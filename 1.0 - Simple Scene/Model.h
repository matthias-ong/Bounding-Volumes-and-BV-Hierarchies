#pragma once

// Include GLEW
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Mesh.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class Model
{
public:
    // model data 
    vector<Mesh> meshes;
    vector<glm::vec3> combinedVertices;
    std::string directory;
    bool gammaCorrection{};

    Model() = default;
    // constructor, expects a filepath to a 3D model.
    Model(std::string const& path, bool gamma = false);

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const& path);
    void loadPoint();
    void loadLine();
    void loadRay();
    void loadTriangle();
    void updateRay(Vertex finalPoint);
    // draws the model, and thus all its meshes
    void Draw();
    void GenericDraw();
    void DrawBoundingVolume();
    void GenericDrawTriangle();
    void updateTriangle(std::vector<Vertex>& newVertices);
    void CleanUp();

private:

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

};
