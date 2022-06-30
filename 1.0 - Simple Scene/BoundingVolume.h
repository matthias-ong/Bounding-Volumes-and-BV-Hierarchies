#pragma once
#include <glm/glm.hpp>
#include <utility>
#include <vector>
#include "Collision.h"
#include "GameObject.h"

namespace BoundingVolume
{
	Collision::AABB createAABB(std::vector<glm::vec3>& vertices);

	std::pair<glm::vec3, glm::vec3> extremePointsAlongDirection(glm::vec3 dir, std::vector<glm::vec3>& vertices);

	std::pair<glm::vec3, glm::vec3> extremePointsAlongXYZ(std::vector<glm::vec3>& vertices);

	void GrowSphere(Collision::Sphere& s, glm::vec3& p);

	Collision::Sphere RitterSphere(std::vector<glm::vec3>& vertices);

	void CovarianceMatrix(glm::mat3& cov, std::vector<glm::vec3>& vertices);
	void SymSchur2(glm::mat3& a, int p, int q, float& c, float& s);
	void Jacobi(glm::mat3& a, glm::mat3& v);
	Collision::Sphere PCASphere(std::vector<glm::vec3>& vertices);

	std::pair<float, float> getExtents(GameObject* obj, char axis, bool renderSphere = false);
	
};

