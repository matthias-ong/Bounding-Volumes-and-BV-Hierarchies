#pragma once
#include <glm/glm.hpp>
#include "Transform.h"
#include "Collision.h"

class GameObject
{
public:
	void SetTransform(Transform trans);
	void SetModelID(const char* id);
	Transform& GetTransform();
	const char* GetModelID();
	void DrawImGuiControls();
	void DrawImGuiBVH();
	bool isCollided() { return collided; }
	void setCollided(bool value) { collided = value;  }
	unsigned int m_id{};
	Transform transform{};

	//BV
	Collision::AABB aabbBV{glm::vec3(0.f), glm::vec3(0.f)};
	Collision::Sphere sphereBV{ glm::vec3(0.f), 0.f };
	const char* colliderName = "AABB"; //when just loading in
	bool changedCollider = true; //if changed, create a new boundingVolume, if not dont create every frame

private:
	const char* model_id{};
	int model_id_int{};
	bool collided{};
	int BV_int{};
	
	
};