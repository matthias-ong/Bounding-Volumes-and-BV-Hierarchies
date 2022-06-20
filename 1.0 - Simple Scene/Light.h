#pragma once
#include <glm/glm.hpp>

struct Light
{
	glm::vec3 lightPos;
	glm::vec3 lightColour{ 1.f, 1.f, 1.f };
};
