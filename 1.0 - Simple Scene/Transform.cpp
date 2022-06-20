#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

Transform::Transform(glm::vec3 pos, float scale, glm::vec3 scale2, glm::vec3 rot, glm::vec3 dir)
	: Position{ pos }, scale{ scale }, scale2{ scale2 }, rotation{ rot }, modelMtx{}, rayDirection {dir}
{
	rotationMtx = glm::mat4(1.f);
	rotationMtx = glm::rotate(rotationMtx, rotation.x, glm::vec3(1.f, 0.f, 0.f));
	rotationMtx = glm::rotate(rotationMtx, rotation.y, glm::vec3(0.f, 1.f, 0.f));
	rotationMtx = glm::rotate(rotationMtx, rotation.z, glm::vec3(0.f, 0.f, 1.f));
	triangleVertices = { {glm::vec3(-0.5f, -0.5f, 0.f)}, {glm::vec3(0.5f, -0.5f, 0.f)},
				 { glm::vec3(0.f, 0.5f, 0.f)} };
}

glm::mat4 Transform::GetModelMtx()
{
	modelMtx = glm::mat4(1.0f);
	modelMtx = glm::translate(modelMtx, this->Position);
	modelMtx = modelMtx * rotationMtx;
	modelMtx = glm::scale(modelMtx, glm::vec3(scale, scale, scale));
	return modelMtx;
}

glm::mat4 Transform::GetModelMtx3f()
{
	modelMtx = glm::mat4(1.0f);
	modelMtx = glm::translate(modelMtx, this->Position);
	modelMtx = modelMtx * rotationMtx;
	modelMtx = glm::scale(modelMtx, glm::vec3(scale2.x, scale2.y, scale2.z));
	return modelMtx;
}