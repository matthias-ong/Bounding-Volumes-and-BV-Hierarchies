#include "Model.h"

Model::Model(std::string const& path, bool gamma) : gammaCorrection(gamma)
{
    loadModel(path);
}

void Model::Draw()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw();
}

void Model::GenericDraw()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].GenericDraw();
}

void Model::DrawBoundingVolume()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].DrawBoundingVolume();
}

void Model::GenericDrawTriangle()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].GenericDrawTriangle();
}

void Model::CleanUp()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].CleanUp();
}

void Model::loadModel(std::string const& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    importer.SetPropertyBool(AI_CONFIG_PP_PTV_NORMALIZE, true);
    const aiScene* scene = importer.ReadFile(path, aiProcess_ValidateDataStructure | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_SortByPType | aiProcess_JoinIdenticalVertices
        | aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates | aiProcess_FindInvalidData | aiProcess_GenNormals | aiProcess_FixInfacingNormals);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);

    if (path.find("quad.obj"))
        loadLine();
}

void Model::loadPoint()
{
    // data to fill
    vector<Vertex> vertices;
    //vector<unsigned int> indices;

    vertices = { {glm::vec3(-0.25f, 0.f, 0.f)}, {glm::vec3(0.25f, 0.f, 0.f)},
                 { glm::vec3(0.f, -0.25f, 0.f)},{ glm::vec3(0.f, 0.25f, 0.f)},
                 { glm::vec3(0.f, 0.f, -0.25f) } , { glm::vec3(0.f, 0.f, 0.25f) }};

    //indices = { 0, 1, 2, 3, 4, 5 };
    Mesh pointMesh{};
    pointMesh.vertices = vertices;
    pointMesh.setupGenericMesh();
    meshes.push_back(pointMesh);
}

void Model::loadLine()
{
    // data to fill
    vector<Vertex> vertices;
    //vector<unsigned int> indices;

    vertices = { {glm::vec3(0.f, 0.f, 0.f)}, {glm::vec3(0.f, 0.f, 1.f)} };

    //indices = { 0, 1, 2, 3, 4, 5 };
    Mesh lineMesh{};
    lineMesh.vertices = vertices;
    lineMesh.setupGenericMesh();
    meshes.push_back(lineMesh);
}

void Model::loadRay()
{
    // data to fill
    vector<Vertex> vertices;
    //vector<unsigned int> indices;
    vertices = { {glm::vec3(-0.25f, 0.f, 0.f)}, {glm::vec3(0.25f, 0.f, 0.f)},
                 { glm::vec3(0.f, -0.25f, 0.f)},{ glm::vec3(0.f, 0.25f, 0.f)},
                 { glm::vec3(0.f, 0.f, -0.25f) } , { glm::vec3(0.f, 0.f, 0.25f) } };

    Mesh pointMesh{};
    pointMesh.vertices = vertices;
    pointMesh.setupGenericMesh();
    meshes.push_back(pointMesh);

    vertices = { {glm::vec3(0.f, 0.f, 0.f)}, {glm::vec3(1.f, 0.f, 0.f)}};

    Mesh rayMesh{};
    rayMesh.vertices = vertices;
    rayMesh.setupGenericMesh();
    meshes.push_back(rayMesh);

    vertices = { {glm::vec3(1-0.1f, 0.f, 0.f)}, {glm::vec3(1+0.1f, 0.f, 0.f)},
                 { glm::vec3(1+0.f, -0.1f, 0.f)},{ glm::vec3(1+0.f, 0.1f, 0.f)},
                 { glm::vec3(1+0.f, 0.f, -0.1f) } , { glm::vec3(1+0.f, 0.f, 0.1f) } };

    Mesh pointMesh2{};
    pointMesh2.vertices = vertices;
    pointMesh2.setupGenericMesh();
    meshes.push_back(pointMesh2);
}

void Model::loadTriangle()
{
    // data to fill
    vector<Vertex> vertices;
    //vector<unsigned int> indices;

    vertices = { {glm::vec3(-0.5f, -0.5f, 0.f)}, {glm::vec3(0.5f, -0.5f, 0.f)},
                 { glm::vec3(0.f, 0.5f, 0.f)} };

    //indices = { 0, 1, 2, 3, 4, 5 };
    Mesh triMesh{};
    triMesh.vertices = vertices;
    triMesh.setupGenericMesh();
    meshes.push_back(triMesh);
}

void Model::updateRay(Vertex finalPoint)
{
    meshes[1].vertices[1] = finalPoint;
    meshes[1].UpdateGenericMesh();

    std::vector<Vertex> vertices = {
        {glm::vec3(finalPoint.Position.x - 0.1f, finalPoint.Position.y, finalPoint.Position.z)},
        {glm::vec3(finalPoint.Position.x + 0.1f, finalPoint.Position.y, finalPoint.Position.z)},
        {glm::vec3(finalPoint.Position.x, finalPoint.Position.y - 0.1f, finalPoint.Position.z)},{glm::vec3(finalPoint.Position.x, finalPoint.Position.y + 0.1f, finalPoint.Position.z)},
        {glm::vec3(finalPoint.Position.x, finalPoint.Position.y , finalPoint.Position.z - 0.1f)},
        {glm::vec3(finalPoint.Position.x, finalPoint.Position.y , finalPoint.Position.z + 0.1f) } };
    meshes[2].vertices = vertices;
    meshes[2].UpdateGenericMesh();
    
}

void Model::updateTriangle(std::vector<Vertex>& newVertices)
{
    
    meshes[0].vertices = newVertices;
    meshes[0].UpdateGenericMesh();
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        vertex.Position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;

        vertex.Normal = vector;

        vertices.push_back(vertex);
        combinedVertices.push_back(vertex.Position);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices);
}