#include "BoundingVolume.h"

namespace BoundingVolume
{

	EPOS currEPOS = EPOS::EPOS8;

	constexpr std::array<glm::vec3, 4> EPOS8 =
	{ glm::vec3{1,1,1}, glm::vec3{1,1,-1}, glm::vec3{1,-1,1}, glm::vec3{1,-1,-1} };

	constexpr std::array<glm::vec3, 6> EPOS12 =
	{ glm::vec3{ 1, 1, 0 }, glm::vec3{ 1, -1, 0 }, glm::vec3{ 1, 0, 1 }, glm::vec3{ 1, 0, -1 },
		glm::vec3{ 0, 1, 1 }, glm::vec3{ 0, 1, -1 } };

	constexpr std::array<glm::vec3, 12> EPOS24 =
	{ glm::vec3{ 0, 1, 2 }, glm::vec3{ 0, 2, 1 }, glm::vec3{1, 0, 2 }, glm::vec3{ 2, 0, 1 },
		glm::vec3{ 1, 2, 0 }, glm::vec3{ 2, 1, 0 },  glm::vec3{ 0, 1, -2 },  glm::vec3{0, 2, -1 },
	 glm::vec3{1, 0, -2}, glm::vec3{ 2, 0, -1 }, glm::vec3{ 1, -2, 0 } , glm::vec3{ 2, -1, 0 } };

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
		Collision::Sphere boundingSphere{ centre,  radius };

		// Grow sphere to include all points (STEP 2)
		for (auto& v : vertices)
			GrowSphere(boundingSphere, v);

		return boundingSphere;
	}

	void CovarianceMatrix(glm::mat3& cov, std::vector<glm::vec3>& vertices)
	{
		float oon = 1.0f / (float)vertices.size();
		glm::vec3 c = glm::vec3(0.0f, 0.0f, 0.0f);
		float e00, e11, e22, e01, e02, e12;
		// Compute the center of mass (centroid) of the points
		for (int i = 0; i < vertices.size(); i++)
			c += vertices[i];
		c *= oon;

		// Compute covariance elements
		e00 = e11 = e22 = e01 = e02 = e12 = 0.0f;
		for (int i = 0; i < vertices.size(); i++) {
			// Translate points so center of mass is at origin
			glm::vec3 p = vertices[i] - c;
			// Compute covariance of translated points
			e00 += p.x * p.x;
			e11 += p.y * p.y;
			e22 += p.z * p.z;
			e01 += p.x * p.y;
			e02 += p.x * p.z;
			e12 += p.y * p.z;
		}
		// Fill in the covariance matrix elements
		cov[0][0] = e00 * oon;
		cov[1][1] = e11 * oon;
		cov[2][2] = e22 * oon;
		cov[0][1] = cov[1][0] = e01 * oon;
		cov[0][2] = cov[2][0] = e02 * oon;
		cov[1][2] = cov[2][1] = e12 * oon;
	}

	// 2-by-2 Symmetric Schur decomposition. Given an n-by-n symmetric matrix
	// and indices p, q such that 1 <= p < q <= n, computes a sine-cosine pair
	// (s, c) that will serve to form a Jacobi rotation matrix.
	// See Golub, Van Loan, Matrix Computations, 3rd ed, p428
	void SymSchur2(glm::mat3& a, int p, int q, float& c, float& s)
	{
		if (abs(a[p][q]) > 0.0001f) {
			float r = (a[q][q] - a[p][p]) / (2.0f * a[p][q]);
			float t;
			if (r >= 0.0f)
				t = 1.0f / (r + sqrt(1.0f + r * r));
			else
				t = -1.0f / (-r + sqrt(1.0f + r * r));
			c = 1.0f / sqrt(1.0f + t * t);
			s = t * c;
		}
		else {
			c = 1.0f;
			s = 0.0f;
		}
	}

	// Computes the eigenvectors and eigenvalues of the symmetric matrix A using
	// the classic Jacobi method of iteratively updating A asA=J∧T * A * J,
	// where J = J(p, q, theta) is the Jacobi rotation matrix.
	//
	// On exit, v will contain the eigenvectors, and the diagonal elements
	// of a are the corresponding eigenvalues.
	//
	// See Golub, Van Loan, Matrix Computations, 3rd ed, p428
	void Jacobi(glm::mat3& a, glm::mat3& v)
	{
		int i, j, n, p, q;
		float prevoff, c, s;
		glm::mat3 J, b, t;
		// Initialize v to identify matrix
		for (i = 0; i < 3; i++) {
			v[i][0] = v[i][1] = v[i][2] = 0.0f;
			v[i][i] = 1.0f;
		}
		// Repeat for some maximum number of iterations
		const int MAX_ITERATIONS = 50;
		for (n = 0; n < MAX_ITERATIONS; n++) {
			// Find largest off-diagonal absolute element a[p][q]
			p = 0; q = 1;
			for (i = 0; i < 3; i++) {
				for (j = 0; j < 3; j++) {
					if (i == j) continue;
					if (abs(a[i][j]) > abs(a[p][q])) {
						p = i;
						q = j;
					}
				}
			}
			// Compute the Jacobi rotation matrix J(p, q, theta)
			// (This code can be optimized for the three different cases of rotation)
			SymSchur2(a, p, q, c, s);
			for (i = 0; i < 3; i++) {
				J[i][0] = J[i][1] = J[i][2] = 0.0f;
				J[i][i] = 1.0f;
			}
			J[p][p] = c; J[p][q] = s;
			J[q][p] = -s; J[q][q] = c;
			// Cumulate rotations into what will contain the eigenvectors
			v = v * J;
			// Make ’a’ more diagonal, until just eigenvalues remain on diagonal
			a = (glm::transpose(J) * a) * J;
			// Compute "norm" of off-diagonal elements
			float off = 0.0f;
			for (i = 0; i < 3; i++) {
				for (j = 0; j < 3; j++) {
					if (i == j) continue;
					off += a[i][j] * a[i][j];
				}
			}
			/* off = sqrt(off); not needed for norm comparison */
			// Stop when norm no longer decreasing
			if (n > 2 && off >= prevoff)
				return;
			prevoff = off;

		}
	}
	Collision::Sphere PCASphere(std::vector<glm::vec3>& vertices)
	{
		glm::mat3 m, v;
		// Compute the covariance matrix m
		CovarianceMatrix(m, vertices);
		// Decompose it into eigenvectors (in v) and eigenvalues (in m)
		Jacobi(m, v);
		// Find the component with largest magnitude eigenvalue (largest spread)
		glm::vec3 e;
		int maxc = 0;
		float maxf, maxe = abs(m[0][0]);
		if ((maxf = abs(m[1][1])) > maxe) maxc = 1, maxe = maxf;
		if ((maxf = abs(m[2][2])) > maxe) maxc = 2, maxe = maxf;
		e[0] = v[0][maxc];
		e[1] = v[1][maxc];
		e[2] = v[2][maxc];
		// Find the most extreme points along direction ’e’
		int imin, imax;
		std::pair<glm::vec3, glm::vec3> result = extremePointsAlongDirection(e, vertices);
		glm::vec3 minpt = result.first;
		glm::vec3 maxpt = result.second;
		float dist = sqrt(glm::dot(maxpt - minpt, maxpt - minpt));
		Collision::Sphere eigenSphere{ (minpt + maxpt) * 0.5f, dist * 0.5f };

		// Grow sphere to include all points (STEP 2)
		for (auto& v : vertices)
			GrowSphere(eigenSphere, v);

		return eigenSphere;
	}

	Collision::Sphere LarssonSphere(std::vector<glm::vec3>& vertices)
	{
		std::vector<glm::vec3> kPoints;
		for (size_t i = 0; i < vertices.size(); i += 5) //Choose a set of K points from the entire set of points
		{
			kPoints.push_back(vertices[i]);
		}

		std::vector<std::pair<glm::vec3, glm::vec3>> extremePoints;
		if (currEPOS == EPOS::EPOS8)
		{
			for (auto& dir : EPOS8)
			{
				extremePoints.emplace_back(extremePointsAlongDirection(dir, kPoints));
			}
		}
		else if (currEPOS == EPOS::EPOS12)
		{
			for (auto& dir : EPOS12)
			{
				extremePoints.emplace_back(extremePointsAlongDirection(dir, kPoints));
			}
		}
		else
		{
			for (auto& dir : EPOS24)
			{
				extremePoints.emplace_back(extremePointsAlongDirection(dir, kPoints));
			}
		}

		

		float greatestDist = -FLT_MAX;
		std::pair<glm::vec3, glm::vec3> widestExtremePoint;
		for (auto&[min, max] : extremePoints) //Structured bindings
		{
			float dist = glm::distance(min, max);
			if (greatestDist < dist)
			{
				greatestDist = dist;
				widestExtremePoint = { min, max };
			}
		}

		// Set up sphere to just encompass these two points
		glm::vec3 centre = (widestExtremePoint.first + widestExtremePoint.second) * 0.5f;
		float radius = glm::dot(widestExtremePoint.second - centre, widestExtremePoint.second - centre);
		radius = sqrt(radius);
		Collision::Sphere boundingSphere{ centre,  radius };

		// Grow sphere to include all points (STEP 2)
		for (auto& v : vertices)
			GrowSphere(boundingSphere, v);

		return boundingSphere;
	}

	std::pair<float, float> getExtents(GameObject* obj, char axis, bool renderSphere)
	{
		std::pair<float, float> result;
		if (renderSphere == false) // get AABB extents
		{
			if (axis == 'x')
			{
				result.first = obj->transform.Position.x - obj->aabbBV.m_Min.x;
				result.second = obj->transform.Position.x + obj->aabbBV.m_Max.x;
			}
			else if (axis == 'y')
			{
				result.first = obj->transform.Position.y - obj->aabbBV.m_Min.y;
				result.second = obj->transform.Position.y + obj->aabbBV.m_Max.y;
			}
			else if (axis == 'z')
			{
				result.first = obj->transform.Position.z - obj->aabbBV.m_Min.z;
				result.second = obj->transform.Position.z + obj->aabbBV.m_Max.z;
			}
		}
		else
		{
			if (axis == 'x')
			{
				result.first = (obj->transform.Position + obj->sphereBV.m_Position - obj->sphereBV.m_Radius * glm::vec3(1.f, 0.f, 0.f)).x;
				result.second = (obj->transform.Position + obj->sphereBV.m_Position + obj->sphereBV.m_Radius * glm::vec3(1.f, 0.f, 0.f)).x;
			}
			if (axis == 'y')
			{
				result.first = (obj->transform.Position + obj->sphereBV.m_Position - obj->sphereBV.m_Radius * glm::vec3(0.f, 1.f, 0.f)).y;
				result.second = (obj->transform.Position + obj->sphereBV.m_Position + obj->sphereBV.m_Radius * glm::vec3(0.f, 1.f, 0.f)).y;
			}
			if (axis == 'z')
			{
				result.first = (obj->transform.Position + obj->sphereBV.m_Position - obj->sphereBV.m_Radius * glm::vec3(0.f, 0.f, 1.f)).z;
				result.second = (obj->transform.Position + obj->sphereBV.m_Position + obj->sphereBV.m_Radius * glm::vec3(0.f, 0.f, 1.f)).z;
			}
		}
		return result;
	}
	EPOS& getCurrEPOS()
	{
		return currEPOS;
	}
}
