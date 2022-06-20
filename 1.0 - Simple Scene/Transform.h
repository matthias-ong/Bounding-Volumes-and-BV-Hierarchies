#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Model.h"

struct Transform
{
	Transform() = default;
	Transform(glm::vec3 pos, float scale = 0.5f, glm::vec3 scale2 = { 0.5f, 0.5f, 0.5f }, glm::vec3 rot = { 0.f, 0.f, 0.f }, glm::vec3 dir = { 1.f, 0.f, 0.f });
	glm::vec3 Position = {0.f, 0.f, 0.f};
	glm::vec3 scale2 = { 1.f, 1.f, 1.f };
	//float scale;
	float scale{1.f};
	glm::vec3 rotation{0.f, 0.f, 0.f}; //angle along 3 axis
	//glm::vec3 Scale = { 0.5f, 0.5f, 0.5f };
	glm::mat4 GetModelMtx();
	glm::mat4 GetModelMtx3f();
	glm::mat4 rotationMtx{};

	//TO DO MOVE THESE AWAY
	glm::vec3 rayDirection{};
	glm::vec3 rayIntersection{};
	std::vector<Vertex> triangleVertices{3};
private:
	glm::mat4 modelMtx;
};