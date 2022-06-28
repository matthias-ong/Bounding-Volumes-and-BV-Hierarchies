#include "Collision.h"
#include <algorithm>
#include <glm/glm.hpp>

bool Collision::PointSphere(const glm::vec3& p, const Sphere& S)
{

	float d = glm::distance(p, S.m_Position);
	float r_sq = (S.m_Radius * S.m_Radius);
	return ((d * d) <= r_sq);
}

bool Collision::SphereSphere(const Sphere& a, const Sphere& b)
{
	return PointSphere(b.m_Position,
		Sphere(a.m_Position,
			a.m_Radius + b.m_Radius));
}

bool Collision::AABBAABB(const AABB& a, const AABB& b)
{
	// for each axis { X, Y, Z }
	for (unsigned int i = 0; i < 3; ++i)
	{
		// if no overlap for the axis, no overlap overall
		if (a.m_Max[i] < b.m_Min[i] || b.m_Max[i] < a.m_Min[i])
			return false;
	}
	return true;
}

// Returns true if sphere s intersects AABB b, false otherwise.
// The point p on the AABB closest to the sphere center is also returned
bool Collision::SphereAABB(const Sphere& a, const AABB& b)
{
	// Find point p on AABB closest to sphere center
	glm::vec3 closestPt = ClosestPtPointAABB(a.m_Position, b);
	// Sphere and AABB intersect if the (squared) distance from sphere
	// center to point p is less than the (squared) sphere radius
	glm::vec3 v = closestPt - a.m_Position;
	return glm::dot(v, v) <= a.m_Radius * a.m_Radius;
}

// Given point p, return the point q on or in AABB b that is closest to p
glm::vec3 Collision::ClosestPtPointAABB(glm::vec3 p, AABB b)
{
	// For each coordinate axis, if the point coordinate value is
	// outside box, clamp it to the box, else keep it as is
	glm::vec3 ret;
	for (int i = 0; i < 3; i++) {
		float v = p[i];
		if (v < b.m_Min[i]) v = b.m_Min[i]; // v = max(v, b.min[i])
		if (v > b.m_Max[i]) v = b.m_Max[i]; // v = min(v, b.max[i])
		ret[i] = v;
	}
	return ret;
}

bool Collision::PointAABB(const glm::vec3& p, const AABB& a)
{
	// for each axis { X, Y, Z }
	for (unsigned int i = 0; i < 3; ++i)
	{
		if (p[i] < a.m_Min[i] || p[i] > a.m_Max[i])
			return false;
	}
	return true;
}

bool Collision::PointPlane(const glm::vec3& p, const Plane& t)
{
	if (glm::dot(t.m_Normal, p) == t.m_d)
		return true; //Lie on plane
	else return false;
	//glm::dot(t.m_Normal, pointToPlane)
}

// Test if AABB b intersects plane p
bool Collision::AABBPlane(const AABB& b, const Plane& p)
{
	// These two lines not necessary with a (center, extents) AABB representation
	glm::vec3 c = (b.m_Max + b.m_Min) * 0.5f; // Compute AABB center
	glm::vec3 e = b.m_Max - c; // Compute positive extents
	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	float r = e[0] * std::abs(p.m_Normal[0]) + e[1] * std::abs(p.m_Normal[1]) + e[2] * std::abs(p.m_Normal[2]);
	// Compute distance of box center from plane
	float s = glm::dot(p.m_Normal, c) - p.m_d;
	// Intersection occurs when distance s falls within [-r,+r] interval
	return std::abs(s) <= r;
}

bool Collision::SpherePlane(const Sphere& s, const Plane& p)
{
	// For a normalized plane (|p.n| = 1), evaluating the plane equation
	// for a point gives the signed distance of the point to the plane
	float dist = glm::dot(s.m_Position, p.m_Normal) - p.m_d;
	// If sphere center within +/-radius from plane, plane intersects sphere
	return std::abs(dist) <= s.m_Radius;
}

inline float TriArea2D(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return (x1 - x2) * (y2 - y3) - (x2 - x3) * (y1 - y2);
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
void Barycentric(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& p, float& u, float& v, float& w)
{
	// Unnormalized triangle normal
	glm::vec3 m = glm::cross(b - a, c - a);
	// Nominators and one-over-denominator for u and v ratios
	float nu, nv, ood;
	// Absolute components for determining projection plane
	float x = std::abs(m.x), y = std::abs(m.y), z = std::abs(m.z);
	// Compute areas in plane of largest projection
	if (x >= y && x >= z) {
		// x is largest, project to the yz plane
		nu = TriArea2D(p.y, p.z, b.y, b.z, c.y, c.z); // Area of PBC in yz plane
		nv = TriArea2D(p.y, p.z, c.y, c.z, a.y, a.z); // Area of PCA in yz plane
		ood = 1.0f / m.x; // 1/(2*area of ABC in yz plane)
	}
	else if (y >= x && y >= z) {
		// y is largest, project to the xz plane
		nu = TriArea2D(p.x, p.z, b.x, b.z, c.x, c.z);
		nv = TriArea2D(p.x, p.z, c.x, c.z, a.x, a.z);
		ood = 1.0f / -m.y;
	}
	else {
		// z is largest, project to the xy plane
		nu = TriArea2D(p.x, p.y, b.x, b.y, c.x, c.y);
		nv = TriArea2D(p.x, p.y, c.x, c.y, a.x, a.y);
		ood = 1.0f / m.z;
	}
	u = nu * ood;
	v = nv * ood;
	w = 1.0f - u - v;
}

// Test if point p is contained in triangle (a, b, c)
bool Collision::PointTriangle(const glm::vec3& p, const Triangle& t)
{
	//Test separating axis corresponding to triangle face normal (category 2)
	glm::vec3 normal = glm::normalize(glm::cross(t.v1, t.v2));
	Plane plane(normal, glm::dot(normal, t.v3));
	if (PointPlane(p, plane)) //if lie on the plane then test for barycentric
	{
		float u, v, w;
		Barycentric(t.v1, t.v2, t.v3, p, u, v, w);
		return v >= 0.0f && w >= 0.0f && (v + w) <= 1.0f;
	}
	return false;

}

glm::vec3 ClosestPtPointTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	// Check if P in vertex region outside A
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ap = p - a;
	float d1 = glm::dot(ab, ap);
	float d2 = glm::dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)
	// Check if P in vertex region outside B
	glm::vec3 bp = p - b;
	float d3 = glm::dot(ab, bp);
	float d4 = glm::dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)
	// Check if P in edge region of AB, if so return projection of P onto AB
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		return a + v * ab; // barycentric coordinates (1-v,v,0)
	}
	// Check if P in vertex region outside C
	glm::vec3 cp = p - c;
	float d5 = glm::dot(ab, cp);
	float d6 = glm::dot(ac, cp);
	if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)
	// Check if P in edge region of AC, if so return projection of P onto AC
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		return a + w * ac; // barycentric coordinates (1-w,0,w)
	}
	// Check if P in edge region of BC, if so return projection of P onto BC
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return b + w * (c - b); // barycentric coordinates (0,1-w,w)
	}
	// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f-v-w
}

bool Collision::SphereTriangle(const Sphere& s, const Triangle& t)
{
	// Find point P on triangle ABC closest to sphere center
	glm::vec3 p = ClosestPtPointTriangle(s.m_Position, t.v1, t.v2, t.v3);
	// Sphere and triangle intersect if the (squared) distance from sphere
// center to point p is less than the (squared) sphere radius
	glm::vec3 v = p - s.m_Position;
	return glm::dot(v, v) <= s.m_Radius * s.m_Radius;

}

bool Collision::TriangleAABB(Triangle& t, const AABB& b)
{
	float p0, p1, p2, r;
	// Compute box center and extents (if not already given in that format)
	glm::vec3 c = (b.m_Min + b.m_Max) * 0.5f;
	float e0 = (b.m_Max.x - b.m_Min.x) * 0.5f;
	float e1 = (b.m_Max.y - b.m_Min.y) * 0.5f;
	float e2 = (b.m_Max.z - b.m_Min.z) * 0.5f;
	// Translate triangle as conceptually moving AABB to origin
	t.v1 = t.v1 - c;
	t.v2 = t.v2 - c;
	t.v3 = t.v3 - c;
	using Vector = glm::vec3;
	// Compute edge vectors for triangle
	Vector f0 = t.v2 - t.v1, f1 = t.v3 - t.v2, f2 = t.v1 - t.v3;

	// Test axes a00..a22 (category 3)
	// Test axis a00
	p0 = t.v1.z * t.v2.y - t.v1.y * t.v2.z;
	p2 = t.v3.z * (t.v2.y - t.v1.y) - t.v3.z * (t.v2.z - t.v1.z);
	r = e1 * std::abs(f0.z) + e2 * std::abs(f0.y);
	if (std::max(-std::max(p0, p2), std::min(p0, p2)) > r) return 0; // Axis is a separating axis
	// Repeat similar tests for remaining axes a01..a22
		// Test the three axes corresponding to the face normals of AABB b (category 1).
		// Exit if...
		// ... [-e0, e0] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
	if (std::max({ t.v1.x, t.v2.x, t.v3.x }) < -e0 || std::min({ t.v1.x, t.v2.x, t.v3.x }) > e0) return 0;
	// ... [-e1, e1] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
	if (std::max({ t.v1.y, t.v2.y, t.v3.y }) < -e1 || std::min({ t.v1.y, t.v2.y, t.v3.y }) > e1) return 0;
	// ... [-e2, e2] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
	if (std::max({ t.v1.z, t.v2.z, t.v3.z }) < -e2 || std::min({ t.v1.z, t.v2.z, t.v3.z }) > e2) return 0;

	//Test separating axis corresponding to triangle face normal (category 2)
	glm::vec3 normal = glm::cross(f0, f1);
	Plane p(normal, glm::dot(normal, t.v1));
	return AABBPlane(b, p);
}

bool Collision::RayPlane(const Ray& ray, const Plane& p, float& t, glm::vec3& q)
{
	// Compute the t value for the directed line ab intersecting the plane
	glm::vec3 ab = ray.m_Direction;
	t = (p.m_d - glm::dot(p.m_Normal, ray.m_Start)) / glm::dot(p.m_Normal, ab);
	// If t in [0..1] compute and return intersection point
	if (t >= 0.0f) {
		q = t * ab; //no need to start from starting point as I will pass this straight to VBO, 0,0 is implied as starting pt
		return 1;
	}
	// Else no intersection
	return 0;
}

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
// returns t value of intersection and intersection point q
bool Collision::RaySphere(const Ray& r, const Sphere& s, float& t, glm::vec3& q)
{
	glm::vec3 m = r.m_Start - s.m_Position;
	float b = glm::dot(m, r.m_Direction);
	float c = glm::dot(m, m) - s.m_Radius * s.m_Radius;
	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
	if (c > 0.0f && b > 0.0f) return 0;
	float discr = b * b - c;
	// A negative discriminant corresponds to ray missing sphere
	if (discr < 0.0f) return 0;
	// Ray now found to intersect sphere, compute smallest t value of intersection
	t = -b - sqrt(discr);
	// If t is negative, ray started inside sphere so clamp t to zero
	if (t < 0.0f) t = 0.0f;
	q = t * r.m_Direction;
	return 1;
}

// Test if segment specified by points p0 and p1 intersects AABB b
bool Collision::TestSegmentAABB(const glm::vec3& p0, const glm::vec3& p1, const AABB& b)
{
	glm::vec3 c = (b.m_Min + b.m_Max) * 0.5f; // Box center-point
	glm::vec3 e = b.m_Max - c; // Box halflength extents
	glm::vec3 m = (p0 + p1) * 0.5f; // Segment midpoint
	glm::vec3 d = p1 - m; // Segment halflength vector
	m = m - c; // Translate box and segment to origin
	// Try world coordinate axes as separating axes
	float adx = std::abs(d.x);
	if (std::abs(m.x) > e.x + adx) return 0;
	float ady = std::abs(d.y);
	if (std::abs(m.y) > e.y + ady) return 0;
	float adz = std::abs(d.z);
	if (std::abs(m.z) > e.z + adz) return 0;
	// Add in an epsilon term to counteract arithmetic errors when segment is
	// (near) parallel to a coordinate axis (see text for detail)
	adx += FLT_EPSILON; ady += FLT_EPSILON; adz += FLT_EPSILON;
	// Try cross products of segment direction vector with coordinate axes
	if (std::abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return 0;
	if (std::abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return 0;
	if (std::abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return 0;
	// No separating axis found; segment must be overlapping AABB
	return 1;
}

// Intersect ray R(t) = p + t*d against AABB a. When intersecting,
// return intersection distance tmin and point q of intersection
bool Collision::RayAABB(const Ray& r, const AABB& a, float& tmin, glm::vec3& q)
{
	tmin = 0.0f; // set to -FLT_MAX to get first hit on line
	float tmax = FLT_MAX; // set to max distance ray can travel (for segment)
// For all three slabs
	for (int i = 0; i < 3; i++) {
		if (std::abs(r.m_Direction[i]) < FLT_EPSILON) {
			// Ray is parallel to slab. No hit if origin not within slab
			if (r.m_Start[i] < a.m_Min[i] || r.m_Start[i] > a.m_Max[i]) return 0;
		}
		else {
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / r.m_Direction[i];
			float t1 = (a.m_Min[i] - r.m_Start[i]) * ood;
			float t2 = (a.m_Max[i] - r.m_Start[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) std::swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 > tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return 0;
		}
	}
	// Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
	tmin += FLT_EPSILON;
	q = r.m_Direction * tmin;
	if (TestSegmentAABB(r.m_Start, r.m_Start + q, a))
		return 1;
	else return 0;
}

//Given Ray p + tq and ccw triangle abc, return whether line pierces triangle. If
//so, also return the barycentric coordinates (u,v,w) of the intersection point
bool Collision::RayTriangle(glm::vec3& p, glm::vec3& q, glm::vec3& a, glm::vec3& b, glm::vec3& c,
	float& u, float& v, float& w, float& t, glm::vec3& result)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	// Compute triangle normal. Can be precalculated or cached if
	// intersecting multiple segments against the same triangle

	bool otherSide = false;
	glm::vec3 n = glm::cross(ac, ab);
	// Compute denominator d. If d <= 0, segment is parallel to or glm::vec3s
	// away from triangle, so exit early
	float d = glm::dot(q, n);
	if (d <= 0.0f)
	{
		n = glm::cross(ab, ac);
		d = glm::dot(q, n);
		otherSide = true;

		if (d <= 0.0f)
			return 0;
	}

	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects iff 0 <= t. Delay dividing by d until intersection has been found to pierce triangle

	glm::vec3 pa = a - p;
	t = glm::dot(pa, n);
	if (t < 0.0f) return 0;

	// Compute barycentric coordinate components and test if within bounds
	glm::vec3 e = glm::cross(q, pa);
	if (otherSide)
		e = glm::cross(pa, q);

	v = glm::dot(ac, e);
	if (v < 0.0f || v > d) return 0;
	w = -glm::dot(ab, e);
	if (w < 0.0f || v + w > d) return 0;

	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f - v - w;
	result = q * t;
	return 1;
}

float Collision::AABB::GetSurfaceArea()
{
	float halfExtentX = m_Max.x - m_Min.x * 0.5f;
	float halfExtentY = m_Max.y - m_Min.y * 0.5f;
	float halfExtentZ = m_Max.z - m_Min.z * 0.5f;
	return abs(8.0f * ((halfExtentX * halfExtentY) + (halfExtentX * halfExtentZ) + (halfExtentY * halfExtentZ)));
}

float Collision::Sphere::GetSurfaceArea()
{
	return 4.f * 3.14159f * m_Radius * m_Radius;
}
