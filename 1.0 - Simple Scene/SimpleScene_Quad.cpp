//
// Created by pushpak on 6/1/18.
//
#include "SimpleScene_Quad.h"
#include <shader.hpp>
#include <glm/vec3.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Transform.h"
#include "Collision.h"
#include "BoundingVolume.h"

bool renderBVHSphere = false;
int indexOfTreeInt = 0;
float nearestNeighbourWeight = 0.6f;
float combinedVolWeight = 0.3f;
float relVolIncreaseWeight = 0.1f;
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::SetupNanoGUI(GLFWwindow* pWindow)
{
	pWindow = nullptr;
}

bool compareX(GameObject* a, GameObject* b)
{
	return a->transform.Position.x < b->transform.Position.x;
}

bool compareY(GameObject* a, GameObject* b)
{
	return a->transform.Position.y < b->transform.Position.y;
}

bool compareZ(GameObject* a, GameObject* b)
{
	return a->transform.Position.z < b->transform.Position.z;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
SimpleScene_Quad::~SimpleScene_Quad()
{
	initMembers();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
SimpleScene_Quad::SimpleScene_Quad(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight),
programID(0), angleOfRotation(0.0f)
{
	initMembers();
}

//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "bugprone-unused-return-value"
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::initMembers()
{
	programID = 0;
	angleOfRotation = 0.0f;
}
//#pragma clang diagnostic pop

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::CleanUp()
{
	// Cleanup VBO
	for (auto model : models)
		model.second.CleanUp();
	glDeleteProgram(programID);
	if (tree != nullptr)
	{
		FreeTree(*tree);
		delete tree;
	}
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Init()
{
	Model sphere;
	sphere.loadModel("Common/models/sphere.obj");
	models.emplace("Sphere", sphere);
	intModelID.emplace(1, "Sphere");

	Model cube;
	cube.loadModel("Common/models/cube.obj");
	models.emplace("Cube", cube);
	intModelID.emplace(2, "Cube");

	Model fourSphere;
	fourSphere.loadModel("Common/models/4Sphere.obj");
	models.emplace("4Sphere", fourSphere);
	intModelID.emplace(3, "4Sphere");

	Model bunny;
	bunny.loadModel("Common/models/bunny.obj");
	models.emplace("Bunny", bunny);
	intModelID.emplace(4, "Bunny");

	Model starWars;
	starWars.loadModel("Common/models/starwars1.obj");
	models.emplace("StarWars", starWars);
	intModelID.emplace(5, "StarWars");

	Model lucyPrinceton;
	lucyPrinceton.loadModel("Common/models/lucy_princeton.obj");
	models.emplace("LucyPrinceton", lucyPrinceton);
	intModelID.emplace(6, "LucyPrinceton");

	//Initialise game objects
	GameObject first;
	first.SetTransform(Transform(glm::vec3{ -4.f, -1.f, -2.f }, 1.f));
	first.SetModelID("4Sphere"); //Sphere object
	gameObjList.push_back(first);
	first.m_id = 0; //Make sure it corresponds to the index of the gameObjList. Can use std::find.

	GameObject second;
	second.SetTransform(Transform(glm::vec3{ 1.5, 0.f, -2.5f }, 1.f));
	second.SetModelID("Bunny"); //Sphere object
	gameObjList.push_back(second);
	second.m_id = 1; //Make sure it corresponds to the index of the gameObjList. Can use std::find.

	GameObject third;
	third.SetTransform(Transform(glm::vec3{ -2.f, 1.f, -4.f }, 1.f));
	third.SetModelID("StarWars"); //Sphere object
	gameObjList.push_back(third);
	third.m_id = 2; //Make sure it corresponds to the index of the gameObjList. Can use std::find.

	GameObject fourth;
	fourth.SetTransform(Transform(glm::vec3{ 1.f, 2.5f, -4.5f }, 1.f));
	fourth.SetModelID("LucyPrinceton"); //Sphere object
	gameObjList.push_back(fourth);
	fourth.m_id = 3; //Make sure it corresponds to the index of the gameObjList. Can use std::find.

	GameObject fifth;
	fifth.SetTransform(Transform(glm::vec3{ -3.f, 4.5f, -5.5f }, 1.f));
	fifth.SetModelID("StarWars"); //Sphere object
	gameObjList.push_back(fifth);
	fifth.m_id = 4; //Make sure it corresponds to the index of the gameObjList. Can use std::find.

	GameObject sixth;
	sixth.SetTransform(Transform(glm::vec3{ -4.f, 1.5f, -6.f }, 1.f));
	sixth.SetModelID("LucyPrinceton"); //Sphere object
	gameObjList.push_back(sixth);
	sixth.m_id = 5; //Make sure it corresponds to the index of the gameObjList. Can use std::find.

	GameObject seventh;
	seventh.SetTransform(Transform(glm::vec3{ -5.f, 2.5f, -3.5f }, 1.f));
	seventh.SetModelID("Bunny"); //Sphere object
	gameObjList.push_back(seventh);
	seventh.m_id = 6; //Make sure it corresponds to the index of the gameObjList. Can use std::find.

	GameObject eighth;
	eighth.SetTransform(Transform(glm::vec3{ 3.f, 1.5f, -2.5f }, 1.f));
	eighth.SetModelID("4Sphere"); //Sphere object
	gameObjList.push_back(eighth);
	eighth.m_id = 6; //Make sure it corresponds to the index of the gameObjList. Can use std::find.

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("Common/shaders/DiffuseShader.vert",
		"Common/shaders/DiffuseShader.frag");

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	miniMapCam.Position = { 0.f, 0.f, 4.f };
	miniMapCam.Zoom = 60.f;


	return Scene::Init();
}

void SimpleScene_Quad::CollisionCheck(GameObject& first, GameObject& second)
{

	//For planes
	first.setCollided(false);
	second.setCollided(false);
	first.GetTransform().rayIntersection = first.GetTransform().rayDirection;
	second.GetTransform().rayIntersection = second.GetTransform().rayDirection;
	if (first.GetModelID() == "Sphere") //1) SPHERE FIRST
	{
		if (second.GetModelID() == "Sphere")
		{
			Collision::Sphere firstSphere(first.GetTransform().Position, first.GetTransform().scale);
			Collision::Sphere secondSphere(second.GetTransform().Position, second.GetTransform().scale);
			if (Collision::SphereSphere(firstSphere, secondSphere))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Cube")
		{
			Collision::Sphere firstSphere(first.GetTransform().Position, first.GetTransform().scale);
			Collision::AABB secondAABB(second.GetTransform().Position - second.GetTransform().scale2, second.GetTransform().Position + second.GetTransform().scale2);
			if (Collision::SphereAABB(firstSphere, secondAABB))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Point")
		{
			Collision::Sphere firstSphere(first.GetTransform().Position, first.GetTransform().scale);
			if (Collision::PointSphere(second.GetTransform().Position, firstSphere))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Plane")
		{
			Collision::Sphere firstSphere(first.GetTransform().Position, first.GetTransform().scale);
			glm::vec3 normal = second.GetTransform().rotationMtx * glm::vec4(0.f, 0.f, 1.f, 1.f); //Apply rotation to the normal here
			Collision::Plane secondPlane(normal, glm::dot(normal, second.GetTransform().Position));
			if (Collision::SpherePlane(firstSphere, secondPlane))
			{
				first.setCollided(true);
				second.setCollided(true);
			}

		}
		else if (second.GetModelID() == "Triangle")
		{
			Collision::Sphere firstSphere(first.GetTransform().Position, first.GetTransform().scale);
			Collision::Triangle secondTriangle(second.GetTransform().triangleVertices[0].Position,
				second.GetTransform().triangleVertices[1].Position, second.GetTransform().triangleVertices[2].Position);
			if (Collision::SphereTriangle(firstSphere, secondTriangle))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Ray")
		{
			Collision::Sphere firstSphere(first.GetTransform().Position, first.GetTransform().scale);
			Collision::Ray secondRay(second.GetTransform().Position, glm::normalize(second.GetTransform().rayDirection));
			float t = 0;
			glm::vec3 q{};
			if (Collision::RaySphere(secondRay, firstSphere, t, second.GetTransform().rayIntersection))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}

	}
	else if (first.GetModelID() == "Cube") //2) CUBE FIRST
	{
		if (second.GetModelID() == "Cube")
		{
			Collision::AABB firstAABB(first.GetTransform().Position - first.GetTransform().scale2, first.GetTransform().Position + first.GetTransform().scale2);
			Collision::AABB secondAABB(second.GetTransform().Position + -second.GetTransform().scale2, second.GetTransform().Position + second.GetTransform().scale2);
			if (Collision::AABBAABB(firstAABB, secondAABB))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Sphere")
		{
			Collision::AABB firstAABB(second.GetTransform().Position - second.GetTransform().scale2, second.GetTransform().Position + second.GetTransform().scale2);
			Collision::Sphere secondSphere(first.GetTransform().Position, first.GetTransform().scale);
			if (Collision::SphereAABB(secondSphere, firstAABB))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Point")
		{
			Collision::AABB firstAABB(first.GetTransform().Position + -first.GetTransform().scale2, first.GetTransform().Position + first.GetTransform().scale2);
			if (Collision::PointAABB(second.GetTransform().Position, firstAABB))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Triangle")
		{
			Collision::AABB firstAABB(first.GetTransform().Position + -first.GetTransform().scale2, first.GetTransform().Position + first.GetTransform().scale2);
			Collision::Triangle secondTriangle(second.GetTransform().triangleVertices[0].Position,
				second.GetTransform().triangleVertices[1].Position, second.GetTransform().triangleVertices[2].Position);
			if (Collision::TriangleAABB(secondTriangle, firstAABB))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Plane")
		{
			Collision::AABB firstAABB(first.GetTransform().Position + -first.GetTransform().scale2, first.GetTransform().Position + first.GetTransform().scale2);
			glm::vec3 normal = second.GetTransform().rotationMtx * glm::vec4(0.f, 0.f, 1.f, 1.f); //Apply rotation to the normal here
			Collision::Plane secondPlane(normal, glm::dot(normal, second.GetTransform().Position));
			if (Collision::AABBPlane(firstAABB, secondPlane))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Ray")
		{
			Collision::AABB firstAABB(first.GetTransform().Position + -first.GetTransform().scale2, first.GetTransform().Position + first.GetTransform().scale2);
			Collision::Ray secondRay(second.GetTransform().Position, glm::normalize(second.GetTransform().rayDirection));
			float t = 0;
			glm::vec3 q{};
			if (Collision::RayAABB(secondRay, firstAABB, t, second.GetTransform().rayIntersection))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
	}
	else if (first.GetModelID() == "Point") //3) POINT FIRST
	{
		if (second.GetModelID() == "Sphere")
		{
			Collision::Sphere secondSphere(second.GetTransform().Position, second.GetTransform().scale);
			if (Collision::PointSphere(first.GetTransform().Position, secondSphere))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Cube")
		{
			Collision::AABB secondAABB(second.GetTransform().Position + -second.GetTransform().scale2, second.GetTransform().Position + second.GetTransform().scale2);
			if (Collision::PointAABB(first.GetTransform().Position, secondAABB))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Plane")
		{
			glm::vec3 normal = second.GetTransform().rotationMtx * glm::vec4(0.f, 0.f, 1.f, 1.f); //Apply rotation to the normal here
			Collision::Plane secondPlane(normal, glm::dot(normal, second.GetTransform().Position));
			if (Collision::PointPlane(first.GetTransform().Position, secondPlane))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Triangle")
		{
			Collision::Triangle secondTriangle(second.GetTransform().triangleVertices[0].Position,
				second.GetTransform().triangleVertices[1].Position, second.GetTransform().triangleVertices[2].Position);
			if (Collision::PointTriangle(first.GetTransform().Position, secondTriangle))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
	}
	else if (first.GetModelID() == "Plane") //4) PLANE FIRST
	{
		if (second.GetModelID() == "Point")
		{
			glm::vec3 normal = first.GetTransform().rotationMtx * glm::vec4(0.f, 0.f, 1.f, 1.f); //Apply rotation to the normal here
			Collision::Plane firstPlane(normal, glm::dot(normal, first.GetTransform().Position));
			if (Collision::PointPlane(second.GetTransform().Position, firstPlane))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Cube")
		{
			glm::vec3 normal = first.GetTransform().rotationMtx * glm::vec4(0.f, 0.f, 1.f, 1.f); //Apply rotation to the normal here
			Collision::Plane firstPlane(normal, glm::dot(normal, first.GetTransform().Position));
			Collision::AABB secondAABB(second.GetTransform().Position + -second.GetTransform().scale2, second.GetTransform().Position + second.GetTransform().scale2);
			if (Collision::AABBPlane(secondAABB, firstPlane))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Sphere")
		{
			glm::vec3 normal = first.GetTransform().rotationMtx * glm::vec4(0.f, 0.f, 1.f, 1.f); //Apply rotation to the normal here
			Collision::Plane firstPlane(normal, glm::dot(normal, first.GetTransform().Position));
			Collision::Sphere secondSphere(second.GetTransform().Position, second.GetTransform().scale);
			if (Collision::SpherePlane(secondSphere, firstPlane))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Ray")
		{
			glm::vec3 normal = first.GetTransform().rotationMtx * glm::vec4(0.f, 0.f, 1.f, 1.f); //Apply rotation to the normal here
			Collision::Plane firstPlane(normal, glm::dot(normal, first.GetTransform().Position));
			Collision::Ray secondRay(second.GetTransform().Position, second.GetTransform().rayDirection);
			float t = 0;
			glm::vec3 q{};
			if (Collision::RayPlane(secondRay, firstPlane, t, second.GetTransform().rayIntersection))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
			else
			{
				first.setCollided(false);
				second.setCollided(false);

			}
		}

	}
	else if (first.GetModelID() == "Triangle") //5) TRIANGLE FIRST
	{
		if (second.GetModelID() == "Point")
		{
			Collision::Triangle firstTriangle(first.GetTransform().triangleVertices[0].Position,
				first.GetTransform().triangleVertices[1].Position, first.GetTransform().triangleVertices[2].Position);
			if (Collision::PointTriangle(second.GetTransform().Position, firstTriangle))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Sphere")
		{
			Collision::Triangle firstTriangle(first.GetTransform().triangleVertices[0].Position,
				first.GetTransform().triangleVertices[1].Position, first.GetTransform().triangleVertices[2].Position);
			Collision::Sphere secondSphere(second.GetTransform().Position, second.GetTransform().scale);
			if (Collision::SphereTriangle(secondSphere, firstTriangle))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Cube")
		{
			Collision::Triangle firstTriangle(first.GetTransform().triangleVertices[0].Position,
				first.GetTransform().triangleVertices[1].Position, first.GetTransform().triangleVertices[2].Position);
			Collision::AABB secondAABB(second.GetTransform().Position + -second.GetTransform().scale2, second.GetTransform().Position + second.GetTransform().scale2);
			if (Collision::TriangleAABB(firstTriangle, secondAABB))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Ray")
		{
			Collision::Triangle firstTriangle(first.GetTransform().triangleVertices[0].Position,
				first.GetTransform().triangleVertices[1].Position, first.GetTransform().triangleVertices[2].Position);
			Collision::Ray secondRay(second.GetTransform().Position, glm::normalize(second.GetTransform().rayDirection));
			float u, v, w, t = 0;
			glm::vec3 q{};
			if (Collision::RayTriangle(second.GetTransform().Position, secondRay.m_Direction, firstTriangle.v1, firstTriangle.v2, firstTriangle.v3, u, v, w, t, second.GetTransform().rayIntersection))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
	}
	else if (first.GetModelID() == "Ray") //6) RAY FIRST
	{
		if (second.GetModelID() == "Plane")
		{
			glm::vec3 normal = second.GetTransform().rotationMtx * glm::vec4(0.f, 0.f, 1.f, 1.f); //Apply rotation to the normal here
			Collision::Plane secondPlane(normal, glm::dot(normal, second.GetTransform().Position));
			Collision::Ray firstRay(first.GetTransform().Position, glm::normalize(first.GetTransform().rayDirection));
			float t = 0;
			glm::vec3 q{};
			if (Collision::RayPlane(firstRay, secondPlane, t, first.GetTransform().rayIntersection))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Sphere")
		{
			Collision::Ray firstRay(first.GetTransform().Position, glm::normalize(first.GetTransform().rayDirection));
			Collision::Sphere secondSphere(second.GetTransform().Position, second.GetTransform().scale);
			float t = 0;
			glm::vec3 q{};
			if (Collision::RaySphere(firstRay, secondSphere, t, first.GetTransform().rayIntersection))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Cube")
		{
			Collision::Ray firstRay(first.GetTransform().Position, glm::normalize(first.GetTransform().rayDirection));
			Collision::AABB secondAABB(second.GetTransform().Position + -second.GetTransform().scale2, second.GetTransform().Position + second.GetTransform().scale2);
			float t = 0;
			glm::vec3 q{};
			if (Collision::RayAABB(firstRay, secondAABB, t, first.GetTransform().rayIntersection))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}
		else if (second.GetModelID() == "Triangle")
		{
			Collision::Ray firstRay(first.GetTransform().Position, glm::normalize(first.GetTransform().rayDirection));
			Collision::Triangle secondTriangle(second.GetTransform().triangleVertices[0].Position,
				second.GetTransform().triangleVertices[1].Position, second.GetTransform().triangleVertices[2].Position);
			float u, v, w, t = 0;
			glm::vec3 q{};
			if (Collision::RayTriangle(first.GetTransform().Position, firstRay.m_Direction, secondTriangle.v1, secondTriangle.v2, secondTriangle.v3, u, v, w, t, first.GetTransform().rayIntersection))
			{
				first.setCollided(true);
				second.setCollided(true);
			}
		}


	}


}

int SimpleScene_Quad::preRender()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glCullFace(GL_BACK);
	return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Render()
{
	glUseProgram(programID);

	glEnableVertexAttribArray(0);

	//Update gameObject attribues (Transform, BV)
	for (int i = 0; i < gameObjList.size(); ++i)
	{
		Transform firstTrans(gameObjList[i].GetTransform().Position, gameObjList[i].GetTransform().scale, gameObjList[i].GetTransform().scale2, gameObjList[i].GetTransform().rotation, gameObjList[i].GetTransform().rayDirection);
		firstTrans.triangleVertices = gameObjList[i].GetTransform().triangleVertices;
		gameObjList[i].SetTransform(firstTrans);
		if (gameObjList[i].changedCollider)
		{
			gameObjList[i].aabbBV = BoundingVolume::createAABB(models[gameObjList[i].GetModelID()].combinedVertices);
			gameObjList[i].aabbBV.m_Min = gameObjList[i].transform.Position + gameObjList[i].aabbBV.m_Min;
			gameObjList[i].aabbBV.m_Max = gameObjList[i].transform.Position + gameObjList[i].aabbBV.m_Max;
			
			if (gameObjList[i].colliderName == "PCA Sphere")
			{
				gameObjList[i].sphereBV = BoundingVolume::PCASphere(models[gameObjList[i].GetModelID()].combinedVertices);
			}
			else if (gameObjList[i].colliderName == "Larsson's EPOS8")
			{
				BoundingVolume::getCurrEPOS() = BoundingVolume::EPOS::EPOS8;
				gameObjList[i].sphereBV = BoundingVolume::LarssonSphere(models[gameObjList[i].GetModelID()].combinedVertices);
			}
			else if (gameObjList[i].colliderName == "Larsson's EPOS12")
			{
				BoundingVolume::getCurrEPOS() = BoundingVolume::EPOS::EPOS12;
				gameObjList[i].sphereBV = BoundingVolume::LarssonSphere(models[gameObjList[i].GetModelID()].combinedVertices);
			}
			else if (gameObjList[i].colliderName == "Larsson's EPOS24")
			{
				BoundingVolume::getCurrEPOS() = BoundingVolume::EPOS::EPOS24;
				gameObjList[i].sphereBV = BoundingVolume::LarssonSphere(models[gameObjList[i].GetModelID()].combinedVertices);
			}
			else //RITTER'S SPHERE
			{
				gameObjList[i].sphereBV = BoundingVolume::RitterSphere(models[gameObjList[i].GetModelID()].combinedVertices);
			}
			gameObjList[i].sphereBV.m_Position = gameObjList[i].transform.Position + gameObjList[i].sphereBV.m_Position; //offset sphere position with gameobj position

			BVHObjs[i] = &gameObjList[i];
			gameObjList[i].changedCollider = false;
		}
		BVHObjs[i] = &gameObjList[i];
	}


	//CollisionCheck(gameObjList[0], gameObjList[1]);
	// ---------- Rendering ----------

	// Camera transform
	glViewport(0, 0, (GLsizei)(camera.width), (GLsizei)(camera.height));
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), camera.width / camera.height, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();

	for (auto& gameObjs : gameObjList)
	{
		Transform gameObjTrans = gameObjs.GetTransform();

		glm::mat4 objTrans;
		if (gameObjs.GetModelID() == "Cube")
			objTrans = projection * view * gameObjTrans.GetModelMtx3f();
		else
			objTrans = projection * view * gameObjTrans.GetModelMtx();
		// Uniform transformation (vertex shader)
		GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");
		glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &objTrans[0][0]);
		glm::vec3 colour{ 0.f, 1.f, 0.f };

		GLint fCamPosLoc = glGetUniformLocation(programID, "cameraPos");
		glUniform3f(fCamPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform1i(glGetUniformLocation(programID, "renderBoundingVolume"), false);
		//Render the model of the gameObjs
		models[gameObjs.GetModelID()].Draw();

		if (renderBV)
		{
			//Render the model of the Bounding Volume
			if (gameObjs.colliderName == "AABB")
			{
				//AABB Update (for now only translation and scale)
				//Set transform for the cube based on the AABB size
				//half extents are the scale
				float scaleX = (gameObjs.aabbBV.m_Max.x - gameObjs.aabbBV.m_Min.x) * 0.5f;
				float scaleY = (gameObjs.aabbBV.m_Max.y - gameObjs.aabbBV.m_Min.y) * 0.5f;
				float scaleZ = (gameObjs.aabbBV.m_Max.z - gameObjs.aabbBV.m_Min.z) * 0.5f;

				glm::vec3 aabbScale{ scaleX, scaleY, scaleZ };
				glm::vec3 aabbCentre = (gameObjs.aabbBV.m_Max + gameObjs.aabbBV.m_Min) / 2.f;
				Transform aabbTrans(aabbCentre, gameObjs.GetTransform().scale, aabbScale, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f });

				objTrans = projection * view * aabbTrans.GetModelMtx3f();
				GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");
				glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &objTrans[0][0]);
				glUniform1i(glGetUniformLocation(programID, "renderBoundingVolume"), true);
				glm::vec3 colour = glm::vec3(0.f, 1.f, 0.f);
				glUniform3f(glGetUniformLocation(programID, "renderColour"), colour.x, colour.y, colour.z);
				//Draw
				models["Cube"].DrawBoundingVolume();
			}
			else if (gameObjs.colliderName == "Ritter's Sphere" || gameObjs.colliderName == "PCA Sphere"
				|| gameObjs.colliderName == "Larsson's EPOS8" || gameObjs.colliderName == "Larsson's EPOS12" || gameObjs.colliderName == "Larsson's EPOS24") //spheres
			{
				float sphereScale{ gameObjs.sphereBV.m_Radius };
				Transform sphereTrans(gameObjs.sphereBV.m_Position, sphereScale, { sphereScale, sphereScale, sphereScale }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f });

				objTrans = projection * view * sphereTrans.GetModelMtx();
				GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");
				glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &objTrans[0][0]);
				glUniform1i(glGetUniformLocation(programID, "renderBoundingVolume"), true);
				glm::vec3 colour = glm::vec3(0.f, 1.f, 0.f);
				glUniform3f(glGetUniformLocation(programID, "renderColour"), colour.x, colour.y, colour.z);
				//Draw
				models["Sphere"].DrawBoundingVolume();
			}
		}
		glDisableVertexAttribArray(0);
	}

	if (BVHenabled)
	{
		if (tree == nullptr)
		{
			//char axis = BVHierarchy::FindLargestAxis(BVHObjs); //Choose the split plane
			//if (axis == 'x')
			//	std::sort(std::begin(BVHObjs), std::end(BVHObjs), &compareX);
			//else if (axis == 'y')
			//	std::sort(std::begin(BVHObjs), std::end(BVHObjs), &compareY);
			//else // z axis
			//	std::sort(std::begin(BVHObjs), std::end(BVHObjs), &compareZ);
			tree = new BVHierarchy::Node*;
			if (bottomUpTree)
			{
				BVHierarchy::BottomUpBVTree(tree, BVHObjs);
				BVHierarchy::SetBottomUpBVTreeDepth(*tree, 0);
			}
			else
			{
				BVHierarchy::TopDownBVTree(tree, BVHObjs, 0, BVHObjs.size() - 1, 0);
			}
		}
		else
		{
			if (!newTree)
				newTree = true;
			RenderTree(tree, projection, view);
		}
	}
	else
	{
		if (tree != nullptr && newTree)
		{
			FreeTree(*tree);
			delete tree;
			tree = nullptr;
			newTree = false;
		}
	}

	//Minimap CODE SECOND DRAW
	{
		//miniMapCam.width = camera.width / 4.f;
		//miniMapCam.height = camera.height / 4.f;
		//glViewport((int)(camera.width - miniMapCam.width), (int)(camera.height - miniMapCam.height), (GLsizei)(miniMapCam.width), (GLsizei(miniMapCam.height)));

	}

	//Render IMGUI controls after rendering the scene
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	{
		ImGui::Begin("Controls");
		if (ImGui::BeginTabBar("Inspector"))
		{
			if (ImGui::BeginTabItem("Bounding Volumes"))
			{
				BVHenabled = false;
				//runOnce = false;
				if (ImGui::Button("Update BV (After moving)"))
				{
					for (auto& obj : gameObjList) //Render gameObj controls
					{
						obj.changedCollider = true;
					}
				}

				ImGui::Text("Render Bounding Volumes");
				ImGui::Checkbox("##RenderBV", &renderBV);

				for (size_t i = 0; i < gameObjList.size(); i++) //Render gameObj controls
				{
					ImGui::Text("Object %d", i+1);
					ImGui::PushID(i);
					gameObjList[i].DrawImGuiControls();
					ImGui::PopID();
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Bounding Volume Hierarchy"))
			{
				BVHenabled = true;
				ImGui::Text("Adjust settings before clicking \"Update Tree\"");
				if (ImGui::Button("Update Tree"))
				{
					if (tree != nullptr)
					{
						FreeTree(*tree);
						delete tree;
						tree = nullptr;
					}
					newTree = true;
				}
				int oldTree = indexOfTreeInt;
				static const char* items[]{ "Top Down Median Split", "Top Down K-EVEN Extents Split", "Top Down Median Extents Split", "Bottom Up Tree" };
				ImGui::NewLine();
				ImGui::Text("Trees");
				ImGui::ListBox("", &indexOfTreeInt, items, IM_ARRAYSIZE(items), 4);
				if (oldTree != indexOfTreeInt && tree != nullptr)
				{
					if (indexOfTreeInt == 3)
						bottomUpTree = true;
					else bottomUpTree = false;
				}
				ImGui::NewLine();
				ImGui::Text("Render Leaves");
				ImGui::Checkbox("##RenderBV", &renderBV);
				bool oldBVH = renderBVHSphere;
				ImGui::Text("Sphere BVH");
				ImGui::Checkbox("##RenderSphere", &renderBVHSphere);
				if (oldBVH != renderBVHSphere && tree != nullptr)
				{
					if (renderBVHSphere) //Purpose of this is to set all the leaves into the right basic BVs
					{
						for (auto& x : gameObjList)
						{
							x.changedCollider = true;
							x.colliderName = "Ritter's Sphere";
						}
					}
					else
					{
						for (auto& x : gameObjList)
						{
							x.changedCollider = true;
							x.colliderName = "AABB";
						}
					}
				}
				ImGui::Text("Render Depth");
				ImGui::InputInt("##RenderDepth", &renderDepth);
				if (renderDepth > 6) renderDepth = 6; //Max height is 7
				if (renderDepth < 0) renderDepth = 0;

				ImGui::Text("Bottom Up Heuristics Weights");
				//ImGui::NextColumn();
				//ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() / 5.0f);
				
				float oldNeighbour = nearestNeighbourWeight;
				ImGui::Text("Nearest Neighbour ");
				ImGui::SameLine();
				ImGui::DragFloat("##NearestNeighbour", &nearestNeighbourWeight, 0.05f, 0.0f, 1.0f, "%.1f");
				if (oldNeighbour != nearestNeighbourWeight) //if modified change other 2 values
				{
					if (nearestNeighbourWeight + combinedVolWeight + relVolIncreaseWeight < 1.f)
					{
						combinedVolWeight = 1.f - nearestNeighbourWeight - relVolIncreaseWeight;
						relVolIncreaseWeight = 1.f - nearestNeighbourWeight - combinedVolWeight;
					}
					else
					{
						combinedVolWeight = 0.f;
						relVolIncreaseWeight = 0.f;
					}
				}

				float oldcombinedVol = combinedVolWeight;
				ImGui::Text("Min Combined Vol ");
				ImGui::SameLine();
				ImGui::DragFloat("##CombinedVol", &combinedVolWeight, 0.05f, 0.0f, 1.0f, "%.1f");
				if (oldcombinedVol != combinedVolWeight) //if modified change other 2 values
				{
					if (nearestNeighbourWeight + combinedVolWeight + relVolIncreaseWeight < 1.f)
					{
						nearestNeighbourWeight = 1.f - combinedVolWeight - relVolIncreaseWeight;
						relVolIncreaseWeight = 1.f - nearestNeighbourWeight - combinedVolWeight;
					}
					else
					{
						nearestNeighbourWeight = 0.f;
						relVolIncreaseWeight = 0.f;
					}
				}
				
				float oldrelIncrease = relVolIncreaseWeight;
				ImGui::Text("Min Rel Increase ");
				ImGui::SameLine();
				ImGui::DragFloat("##RelativeIncrease", &relVolIncreaseWeight, 0.05f, 0.0f, 1.0f, "%.1f");
				if (oldrelIncrease != relVolIncreaseWeight) //if modified change other 2 values
				{
					if (nearestNeighbourWeight + combinedVolWeight + relVolIncreaseWeight < 1.f)
					{
						nearestNeighbourWeight = 1.f - combinedVolWeight - relVolIncreaseWeight;
						combinedVolWeight = 1.f - nearestNeighbourWeight - relVolIncreaseWeight;
					}
					else
					{
						nearestNeighbourWeight = 0.f;
						combinedVolWeight = 0.f;
					}
				}
				
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}


		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::postRender()
{
	//angleOfRotation += 0.001f;

	return 0;
}

void SimpleScene_Quad::RenderTree(BVHierarchy::Node** tree, const glm::mat4& projection, const glm::mat4& view)
{

	BVHierarchy::Node* node = *tree;
	if (node == nullptr)
		return;
	if (node->treeDepth > renderDepth)
		return; //dont render the deeper nodes

	Transform aabbTrans, sphereTrans;
	if (renderBVHSphere)
	{
		glm::vec3 sphereScale{ node->BV_Sphere.m_Radius, node->BV_Sphere.m_Radius, node->BV_Sphere.m_Radius };
		Transform temp(node->BV_Sphere.m_Position, node->BV_Sphere.m_Radius, sphereScale, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f });
		sphereTrans = temp;
	}
	else
	{
		float scaleX = (node->BV_AABB.m_Max.x - node->BV_AABB.m_Min.x) * 0.5f;
		float scaleY = (node->BV_AABB.m_Max.y - node->BV_AABB.m_Min.y) * 0.5f;
		float scaleZ = (node->BV_AABB.m_Max.z - node->BV_AABB.m_Min.z) * 0.5f;

		glm::vec3 aabbScale{ scaleX, scaleY, scaleZ };
		glm::vec3 aabbCentre = (node->BV_AABB.m_Max + node->BV_AABB.m_Min) / 2.f;
		Transform temp(aabbCentre, 1.f, aabbScale, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f });
		aabbTrans = temp;
	}


	//gameObjs.SetTransform(aabbTrans);
	glm::mat4 objTrans;
	if (renderBVHSphere)
		objTrans = projection * view * sphereTrans.GetModelMtx();
	else
		objTrans = projection * view * aabbTrans.GetModelMtx3f();
	GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");
	glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &objTrans[0][0]);
	glUniform1i(glGetUniformLocation(programID, "renderBoundingVolume"), true);
	
	//TreeDepth 0 Root Node
	glm::vec3 colour = glm::vec3(1.f, 0.f, 0.f); //Red
	if (node->treeDepth == 1)
		colour = glm::vec3(1.f, 0.5f, 1.f); //Orange
	else if (node->treeDepth == 2)
		colour = glm::vec3(1.f, 1.f, 0.f); //Yellow
	else if (node->treeDepth == 3)
		colour = glm::vec3(0.f, 1.f, 1.f); //Light Blue
	else if (node->treeDepth == 4)
		colour = glm::vec3(0.f, 0.f, 1.f); //Blue
	else if (node->treeDepth == 5)
		colour = glm::vec3(1.f, 0.f, 1.f); //Pink
	else if (node->treeDepth == 6)
		colour = glm::vec3(0.5f, 0.5f, 0.5f); //Grey

	//	colour = glm::vec3(0.f, 0.f, 1.f);
	glUniform3f(glGetUniformLocation(programID, "renderColour"), colour.x, colour.y, colour.z);
	//Draw
	if (renderBVHSphere)
		models["Sphere"].DrawBoundingVolume();
	else models["Cube"].DrawBoundingVolume();
	RenderTree(&node->lChild, projection, view);
	RenderTree(&node->rChild, projection, view);
}

void SimpleScene_Quad::FreeTree(BVHierarchy::Node* node)
{
	if (node == nullptr)
		return;
	//treeDepth = 0;
	FreeTree(node->lChild);
	FreeTree(node->rChild);
	delete node;

}
