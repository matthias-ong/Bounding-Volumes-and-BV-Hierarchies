#pragma once
#include <glm/glm.hpp>
#include <utility>
#include <vector>
#include "Collision.h"

namespace BoundingVolume
{
	Collision::AABB createAABB(std::vector<glm::vec3>& vertices);

	std::pair<glm::vec3, glm::vec3> extremePointsAlongDirection(glm::vec3 dir, std::vector<glm::vec3>& vertices);

	std::pair<glm::vec3, glm::vec3> extremePointsAlongXYZ(std::vector<glm::vec3>& vertices);

	void GrowSphere(Collision::Sphere& s, glm::vec3& p);

	Collision::Sphere RitterSphere(std::vector<glm::vec3>& vertices);
};

