#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Collision
{
	struct Collider{};
	struct Plane : Collider
	{
		glm::vec3 m_Normal; //normalized
		float m_d; // d = dot(n,p) for a given point p on the plane
		Plane(glm::vec3 normal, float d) : m_Normal{ normal }, m_d {d} {}
	};

	struct Triangle : Collider
	{
		glm::vec3 v1;
		glm::vec3 v2;
		glm::vec3 v3;
		Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
			: v1 { v1 }, v2 { v2 },
			  v3 { v3 } {}
	};

	struct Sphere : Collider
	{
		glm::vec3 m_Position;
		float m_Radius;
		Sphere(glm::vec3 pos, float radius) : m_Position{ pos }, m_Radius{ radius } {};
		float GetSurfaceArea();
	};

	struct AABB : Collider
	{
		glm::vec3 m_Min;
		glm::vec3 m_Max;
		AABB(glm::vec3 min, glm::vec3 max) : m_Min{ min }, m_Max{ max } {}
		float GetSurfaceArea();
	};

	struct Ray : Collider
	{
		glm::vec3 m_Start;
		glm::vec3 m_Direction;
		Ray(glm::vec3 start, glm::vec3 dir) : m_Start{ start }, m_Direction{ dir } {}
	};
	bool PointSphere(const glm::vec3& p, const Sphere& S);
	bool SphereSphere(const Sphere& a, const Sphere& b);
	bool AABBAABB(const AABB& a, const AABB& b);
	bool SphereAABB(const Sphere& a, const AABB& b);
	bool PointAABB(const glm::vec3& p, const AABB& a);
	bool PointPlane(const glm::vec3& p, const Plane& t);
	bool AABBPlane(const AABB& b, const Plane& p);
	bool SpherePlane(const Sphere& s, const Plane& p);
	bool PointTriangle(const glm::vec3& p, const Triangle& t);
	bool SphereTriangle(const Sphere& s, const Triangle& t);
	bool TriangleAABB(Triangle& t, const AABB& b);
	bool RayPlane(const Ray& ray, const Plane& p, float& t, glm::vec3& q);
	bool RaySphere(const Ray& r, const Sphere& s, float& t, glm::vec3& q);
	bool RayAABB(const Ray& r, const AABB& a, float& tmin, glm::vec3& q);
	bool TestSegmentAABB(const glm::vec3& p0, const glm::vec3& p1, const AABB& b);
	glm::vec3 ClosestPtPointAABB(glm::vec3 p, AABB b);
	bool RayTriangle(glm::vec3& p, glm::vec3& q, glm::vec3& a, glm::vec3& b, glm::vec3& c,
		float& u, float& v, float& w, float& t, glm::vec3& result);

	//AABB createAABB(std::vector<glm::vec3>& vertices);
	//std::pair<glm::vec3, glm::vec3> extremePointsAlongDirection(glm::vec3 dir, std::vector<glm::vec3>& vertices);
};