#include "BVHierarchy.h"
#include "SimpleScene_Quad.h"
#define MEDIAN_CUT

namespace BVHierarchy
{
	bool compareX(GameObject* a, GameObject* b)
	{
		return a->transform.Position.x < b->transform.Position.x;
	}

	bool compareY(GameObject* a, GameObject* b)
	{
		return a->transform.Position.y < b->transform.Position.y;
	}

	bool compareZ(GameObject* a, GameObject* b)
	{
		return a->transform.Position.z < b->transform.Position.z;
	}

	char FindLargestAxis(std::vector<GameObject*>& objects) //decides the split plane
	{
		float minX = FLT_MAX, maxX = -FLT_MAX;
		float minY = FLT_MAX, maxY = -FLT_MAX;
		float minZ = FLT_MAX, maxZ = -FLT_MAX;
		for (int i = 0; i < objects.size(); ++i)
		{
			if (objects[i]->transform.Position.x < minX) minX = objects[i]->transform.Position.x;
			if (objects[i]->transform.Position.y < minY) minY = objects[i]->transform.Position.y;
			if (objects[i]->transform.Position.z < minZ) minZ = objects[i]->transform.Position.z;

			if (objects[i]->transform.Position.x > maxX) maxX = objects[i]->transform.Position.x;
			if (objects[i]->transform.Position.y > maxY) maxY = objects[i]->transform.Position.y;
			if (objects[i]->transform.Position.z > maxZ) maxZ = objects[i]->transform.Position.z;
		}

		float xSpread = abs(maxX - minX);
		float ySpread = abs(maxY - minY);
		float zSpread = abs(maxZ - minZ);
		if (xSpread > ySpread && xSpread > zSpread)
			return 'x';
		else if (ySpread > xSpread && ySpread > zSpread)
			return 'y';
		else return 'z';
	}

	//AABB Method
	Collision::AABB ComputeBoundingVolume(std::vector<GameObject*>& objects, int startIndex, int numObjects)
	{
		//assert(numObjects > 0);
		glm::vec3 Min = objects[startIndex]->transform.Position + objects[startIndex]->aabbBV.m_Min;
		glm::vec3 Max = objects[startIndex]->transform.Position + objects[startIndex]->aabbBV.m_Max;
		for (int i = startIndex + 1; i <= numObjects; ++i)
		{
			if (Min.x > objects[i]->transform.Position.x + objects[i]->aabbBV.m_Min.x)
				Min.x = objects[i]->transform.Position.x + objects[i]->aabbBV.m_Min.x;
			if (Max.x < objects[i]->transform.Position.x + objects[i]->aabbBV.m_Max.x)
				Max.x = objects[i]->transform.Position.x + objects[i]->aabbBV.m_Max.x;
			if (Min.y > objects[i]->transform.Position.y + objects[i]->aabbBV.m_Min.y)
				Min.y = objects[i]->transform.Position.y + objects[i]->aabbBV.m_Min.y;
			if (Max.y < objects[i]->transform.Position.y + objects[i]->aabbBV.m_Max.y)
				Max.y = objects[i]->transform.Position.y + objects[i]->aabbBV.m_Max.y;
			if (Min.z > objects[i]->transform.Position.z + objects[i]->aabbBV.m_Min.z)
				Min.z = objects[i]->transform.Position.z + objects[i]->aabbBV.m_Min.z;
			if (Max.z < objects[i]->transform.Position.z + objects[i]->aabbBV.m_Max.z)
				Max.z = objects[i]->transform.Position.z + objects[i]->aabbBV.m_Max.z;
		}
		return Collision::AABB(Min, Max);
	}

	Collision::Sphere ComputeBoundingSphere(std::vector<GameObject*>& objects, int startIndex, int numObjects)
	{
		//Collision::Sphere s{ objects[startIndex]->sphereBV.m_Position ,
		//	objects[startIndex]->sphereBV.m_Radius };

		//if (numObjects - startIndex > 0)
		//{
		//	std::vector<GameObject*> temp;
		//	for (int i = startIndex; i <= numObjects; ++i)
		//	{
		//		temp.push_back(objects[i]);
		//	}
		//	char axis = FindLargestAxis(temp);
		//	if (axis == 'x')
		//		std::sort(std::begin(temp), std::end(temp), &compareX);
		//	else if (axis == 'y')
		//		std::sort(std::begin(temp), std::end(temp), &compareY);
		//	else // z axis
		//		std::sort(std::begin(temp), std::end(temp), &compareZ);

		//	glm::vec3 secondPos = temp[temp.size() - 1]->sphereBV.m_Position;
		//	glm::vec3 dir = glm::normalize(secondPos - s.m_Position);

		//	glm::vec3 min = s.m_Position - s.m_Radius * dir;
		//	glm::vec3 max = secondPos + temp[temp.size() - 1]->sphereBV.m_Radius;

		//	s.m_Position = min + 0.5f * (max - min);
		//	s.m_Radius = 0.5f * glm::distance(min, max);

		//}
		Collision::Sphere s{ objects[startIndex]->transform.Position + objects[startIndex]->sphereBV.m_Position,
			objects[startIndex]->sphereBV.m_Radius };
		for (size_t i = (size_t)startIndex + 1; i <= numObjects; ++i)
		{
			glm::vec3 secondPos = objects[i]->transform.Position + objects[i]->sphereBV.m_Position;
			glm::vec3 dir = glm::normalize(secondPos - s.m_Position);

			glm::vec3 edgeOfsecond = secondPos + objects[i]->sphereBV.m_Radius * dir;

			//s.m_Position = min + 0.5f * (max - min);
			//s.m_Radius = glm::distance(min, max) * 0.5f;
			BoundingVolume::GrowSphere(s, edgeOfsecond);
		}
		return s;
	}

	void TopDownBVTree(Node** tree, std::vector<GameObject*>& objects, int startIndex, int endIndex, int depth)
	{
		int numObjects = endIndex - startIndex + 1;
		//if (startIndex == endIndex)
		//	numObjects = 1;
		const int MIN_OBJECTS_PER_LEAF = 1;
		Node* pNode = new Node;
		*tree = pNode;
		// Compute a bounding volume for object[split], ..., object[numObjects - 1]
		if (renderBVHSphere)
		{
			pNode->BV_Sphere = ComputeBoundingSphere(objects, startIndex, endIndex);

		}
		else
			pNode->BV_AABB = ComputeBoundingVolume(objects, startIndex, endIndex);

		if (numObjects <= MIN_OBJECTS_PER_LEAF) {
			pNode->type = Node::Type::LEAF;
			pNode->numObjects = numObjects;
			pNode->data = objects[startIndex]; // Pointer to first object in leaf
			pNode->treeDepth = depth;
		}
		else {
			pNode->type = Node::Type::INTERNAL;
			pNode->treeDepth = depth;
			// Based on some partitioning strategy, arrange objects into
			// two partitions: object[0..k], and object[k+1..numObjects-1]
			int k = PartitionObjects(objects, startIndex, endIndex);
			// Recursively construct left and right subtree from subarrays and
			// point the left and right fields of the current node at the subtrees
			TopDownBVTree(&(pNode->lChild), objects, startIndex, k, depth + 1);
			TopDownBVTree(&(pNode->rChild), objects, k + 1, endIndex, depth + 1);
		}

		//Recompute parent nodes to fit children spheres
		if (renderBVHSphere && pNode->lChild && pNode->rChild)
			pNode->BV_Sphere = RecomputeParentSphere(pNode->BV_Sphere, pNode->lChild->BV_Sphere, pNode->rChild->BV_Sphere);

	}

	//Chosen Heuristic: minimise total volume occupied by child nodes (volume proportional to SA)
	float GetHeuristicCost(std::vector<GameObject*>& objects, // the array of objects
		int startIndex,
		int split, // the index to split into left & right
		int numObjects) // the total number of objects
	{
		if (renderBVHSphere)
		{
			Collision::Sphere leftSphere = ComputeBoundingSphere(objects, startIndex, split);
			float leftSurfaceArea = leftSphere.GetSurfaceArea();
			Collision::Sphere rightSphere = ComputeBoundingSphere(objects, split + 1, numObjects);
			float rightSurfaceArea = rightSphere.GetSurfaceArea();
			return (split * leftSurfaceArea) +
				((numObjects - split - 1) * rightSurfaceArea);
		}
		else
		{
			/*Collision::AABB parentAABB = ComputeBoundingVolume(&objects[0], numObjects);
		float parentSurfaceArea = parentAABB.GetSurfaceArea();*/
			Collision::AABB leftAABB = ComputeBoundingVolume(objects, startIndex, split);
			float leftSurfaceArea = leftAABB.GetSurfaceArea();
			Collision::AABB rightAABB = ComputeBoundingVolume(objects, split + 1, numObjects);
			float rightSurfaceArea = rightAABB.GetSurfaceArea();
			return (split * leftSurfaceArea) +
				((numObjects - split - 1) * rightSurfaceArea);
		}

	}

	int PartitionObjects(std::vector<GameObject*>& objects, int startIndex, int endIndex)
	{
		int numObjects = endIndex - startIndex;
		float costX, costY, costZ;
#ifdef MEDIAN_CUT
		std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex, &compareX); //'X' PLANE
		costX = GetHeuristicCost(objects, startIndex, startIndex + numObjects / 2.f, numObjects);
		std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex, &compareY); //'Y' PLANE
		costY = GetHeuristicCost(objects, startIndex, startIndex + numObjects / 2.f, numObjects);
		std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex, &compareZ); //'Z' PLANE
		costZ = GetHeuristicCost(objects, startIndex, startIndex + numObjects / 2.f, numObjects);

		if (costX < costY && costX < costZ)
		{
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex, &compareX); //'X' PLANE
		}
		else if (costY < costX && costY < costZ)
		{
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex, &compareY); //'Y' PLANE
		}
		// else already sorted in z axis
		return startIndex + numObjects / 2.f; //MEDIAN SPLIT
#else //HEURISTIC SPLIT: Find the best split point
		float split = 0;
		float minCost = FLT_MAX;

		for (int i = startIndex + 1; i < numberOfObjects; ++i)
		{
			//possible split combinations
			//index 0 and 1 to numObjects - 1;
			//index 0 to 1 AND 2 to numObjects - 1;
			//index 0 to 2, 3 to numObjects - 1;
			//Divide the space into two subsets
			float cost = GetHeuristicCost(objects, startIndex, i, numObjects);
			if (cost < minCost)
			{
				minCost = cost; //take the lowest cost one
				split = i; //Find the best split point
			}

		}
		return split;
#endif
	}

	Collision::Sphere RecomputeParentSphere(Collision::Sphere& parent, Collision::Sphere& lchild, Collision::Sphere& rchild)
	{
		Collision::Sphere s{ parent.m_Position, parent.m_Radius };
		glm::vec3 secondPos = lchild.m_Position;
		glm::vec3 dir = glm::normalize(secondPos - s.m_Position);
		glm::vec3 edgeOfsecond = secondPos + lchild.m_Radius * dir;
		BoundingVolume::GrowSphere(s, edgeOfsecond);

		secondPos = rchild.m_Position;
		dir = glm::normalize(secondPos - s.m_Position);
		edgeOfsecond = secondPos + rchild.m_Radius * dir;
		BoundingVolume::GrowSphere(s, edgeOfsecond);
		return s;
	}

	//void RenderTopDownBVTree(Node** tree)
	//{
	//	
	//}

}

