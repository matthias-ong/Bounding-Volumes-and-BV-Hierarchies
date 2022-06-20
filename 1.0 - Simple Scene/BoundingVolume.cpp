#include "BoundingVolume.h"

namespace BoundingVolume
{
	Collision::AABB createAABB(std::vector<glm::vec3>& vertices)
	{
		glm::vec3 Min = glm::vec3(FLT_MAX);
		glm::vec3 Max = glm::vec3(-FLT_MAX);
		for (unsigned int i = 0; i < vertices.size(); ++i)
		{
			glm::vec3 pt = vertices.at(i);
			Min.x = std::min(Min.x, vertices[i].x);
			Min.y = std::min(Min.y, vertices[i].y);
			Min.z = std::min(Min.z, vertices[i].z);
			Max.x = std::max(Max.x, vertices[i].x);
			Max.y = std::max(Max.y, vertices[i].y);
			Max.z = std::max(Max.z, vertices[i].z);
		}
		return Collision::AABB(Min, Max);
	}

	std::pair<glm::vec3, glm::vec3> extremePointsAlongDirection(glm::vec3 dir,
		std::vector<glm::vec3>& vertices)
	{
		// Assign Min and Max as before
		float minProj = FLT_MAX, maxProj = -FLT_MAX;

		glm::vec3 Min;
		glm::vec3 Max;
		for (const auto& v : vertices)
		{
			float projDist = glm::dot(dir, v);
			if (minProj > projDist)
			{
				minProj = projDist;
				Min = v;
			}
			if (maxProj <= projDist)
			{
				maxProj = projDist;
				Max = v;
			}
		}
		return { Min, Max };
	}

	std::pair<glm::vec3, glm::vec3> extremePointsAlongXYZ(std::vector<glm::vec3>& vertices)
	{
		// First find most extreme points along principal axes
		int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
		for (int i = 1; i < vertices.size(); ++i)
		{
			if (vertices[i].x < vertices[minx].x) minx = i;
			if (vertices[i].x > vertices[maxx].x) maxx = i;
			if (vertices[i].y < vertices[miny].y) miny = i;
			if (vertices[i].y > vertices[maxy].y) maxy = i;
			if (vertices[i].z < vertices[minz].z) minz = i;
			if (vertices[i].z > vertices[maxz].z) maxz = i;
		}

		// Compute the squared distances for the three pairs of points
		float dist2x = glm::dot(vertices[maxx] - vertices[minx], vertices[maxx] - vertices[minx]);
		float dist2y = glm::dot(vertices[maxy] - vertices[miny], vertices[maxy] - vertices[miny]);
		float dist2z = glm::dot(vertices[maxz] - vertices[minz], vertices[maxz] - vertices[minz]);
		// Pick the pair (min,max) of points most distant
		int min = minx;
		int max = maxx;
		if (dist2y > dist2x && dist2y > dist2z) {
			max = maxy;
			min = miny;
		}
		if (dist2z > dist2x && dist2z > dist2y) {
			max = maxz;
			min = minz;
		}
		return { vertices[min], vertices[max] };
	}

	// Given Sphere s and Point p, update s (if needed) to just encompass p
	void GrowSphere(Collision::Sphere& s, glm::vec3& p)
	{
		// Compute squared distance between point and sphere center
		glm::vec3 d = p - s.m_Position;
		float dist2 = glm::dot(d, d);
		// Only update s if point p is outside it
		if (dist2 > s.m_Radius * s.m_Radius) {
			float dist = sqrt(dist2);
			float newRadius = (s.m_Radius + dist) * 0.5f;
			float k = (newRadius - s.m_Radius) / dist;
			s.m_Radius = newRadius;
			s.m_Position += d * k;
		}
	}

	Collision::Sphere RitterSphere(std::vector<glm::vec3>& vertices)
	{
		// Get sphere encompassing two approximately most distant points
		// Find the most separated point pair
		std::pair<glm::vec3, glm::vec3> extremePoints = extremePointsAlongXYZ(vertices);
		// Set up sphere to just encompass these two points
		glm::vec3 centre = (extremePoints.first + extremePoints.second) * 0.5f;
		float radius = glm::dot(extremePoints.second - centre, extremePoints.second - centre);
		radius = sqrt(radius);
		Collision::Sphere boundingSphere{ centre,  radius};

		// Grow sphere to include all points (STEP 2)
		for (auto& v : vertices)
			GrowSphere(boundingSphere, v);

		return boundingSphere;
	}
}
