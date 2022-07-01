#include "BVHierarchy.h"
#include "SimpleScene_Quad.h"

#define K_CUTS 10.f //10-even SPLITS

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
		glm::vec3 Min = objects[startIndex]->aabbBV.m_Min;
		glm::vec3 Max = objects[startIndex]->aabbBV.m_Max;
		for (int i = startIndex + 1; i <= numObjects; ++i)
		{
			if (Min.x > objects[i]->aabbBV.m_Min.x)
				Min.x = objects[i]->aabbBV.m_Min.x;
			if (Max.x < objects[i]->aabbBV.m_Max.x)
				Max.x = objects[i]->aabbBV.m_Max.x;
			if (Min.y > objects[i]->aabbBV.m_Min.y)
				Min.y = objects[i]->aabbBV.m_Min.y;
			if (Max.y < objects[i]->aabbBV.m_Max.y)
				Max.y = objects[i]->aabbBV.m_Max.y;
			if (Min.z > objects[i]->aabbBV.m_Min.z)
				Min.z = objects[i]->aabbBV.m_Min.z;
			if (Max.z < objects[i]->aabbBV.m_Max.z)
				Max.z = objects[i]->aabbBV.m_Max.z;
		}
		return Collision::AABB(Min, Max);
	}

	Collision::Sphere ComputeBoundingSphere(std::vector<GameObject*>& objects, int startIndex, int numObjects)
	{
		Collision::Sphere s{ objects[startIndex]->sphereBV.m_Position,
			objects[startIndex]->sphereBV.m_Radius };
		for (size_t i = (size_t)startIndex + 1; i <= numObjects; ++i)
		{
			glm::vec3 secondPos = objects[i]->sphereBV.m_Position;
			glm::vec3 dir = glm::normalize(secondPos - s.m_Position);

			glm::vec3 edgeOfsecond = secondPos + objects[i]->sphereBV.m_Radius * dir;
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
		if (indexOfTreeInt == (int)Tree::TOP_DOWN_MEDIAN_SPLIT)
		{
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareX); //'X' PLANE
			costX = GetHeuristicCost(objects, startIndex, startIndex + numObjects / 2.f, numObjects);
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareY); //'Y' PLANE
			costY = GetHeuristicCost(objects, startIndex, startIndex + numObjects / 2.f, numObjects);
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareZ); //'Z' PLANE
			costZ = GetHeuristicCost(objects, startIndex, startIndex + numObjects / 2.f, numObjects);

			if (costX < costY && costX < costZ)
			{
				std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareX); //'X' PLANE
			}
			else if (costY < costX && costY < costZ)
			{
				std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareY); //'Y' PLANE
			}
			// else already sorted in z axis
			return startIndex + numObjects / 2.f; //MEDIAN SPLIT
		}
		else if (indexOfTreeInt == (int)Tree::TOP_DOWN_K_EVEN_SPLIT)
		{
			int indexX, indexY, indexZ;
			//first test which K-even cuts, whether along X,Y,Z axis produces smaller heuristic
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareX); //'X' PLANE
			float range = abs(objects[startIndex]->transform.Position.x - objects[endIndex]->transform.Position.x);
			float splitSize = range / K_CUTS; // 10-even splits
			float splitPointX = objects[startIndex]->transform.Position.x + splitSize * K_CUTS * 0.5f; //Middle of the K_EVEN CUTS
			indexX = FindIndexClosestToPoint(objects, splitPointX, startIndex, endIndex, 'x');
			costX = GetHeuristicCost(objects, startIndex, indexX, numObjects);

			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareY); //'Y' PLANE
			range = abs(objects[startIndex]->transform.Position.y - objects[endIndex]->transform.Position.y);
			splitSize = range / K_CUTS; // 10-even splits
			float splitPointY = objects[startIndex]->transform.Position.y + splitSize * K_CUTS * 0.5f; //Middle of the K_EVEN CUTS
			indexY = FindIndexClosestToPoint(objects, splitPointY, startIndex, endIndex, 'y');
			costY = GetHeuristicCost(objects, startIndex, indexY, numObjects);

			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareZ); //'Z' PLANE
			range = abs(objects[startIndex]->transform.Position.z - objects[endIndex]->transform.Position.z);
			splitSize = range / K_CUTS; // 10-even splits
			float splitPointZ = objects[startIndex]->transform.Position.z + splitSize * K_CUTS * 0.5f; //Middle of the K_EVEN CUTS
			indexZ = FindIndexClosestToPoint(objects, splitPointZ, startIndex, endIndex, 'z');
			costZ = GetHeuristicCost(objects, startIndex, indexZ, numObjects);

			if (costX < costY && costX < costZ)
			{
				std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareX); //'X' PLANE
				return indexX;
			}
			else if (costY < costX && costY < costZ)
			{
				std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareY); //'Y' PLANE
				return indexY;
			}
			// else already sorted in z axis
			return indexZ; //K-EVEN SPLIT
		}
		else if (indexOfTreeInt == (int)Tree::TOP_DOWN_MEDIAN_EXTENTS_SPLIT)
		{
			if (endIndex - startIndex <= 1)
				return startIndex; //too little objects
			int indexX, indexY, indexZ;
			std::vector<float> extents;
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareX); //'X' PLANE
			for (int i = startIndex; i <= endIndex; ++i)
			{
				std::pair<float, float> extent = BoundingVolume::getExtents(objects[i], 'x', renderBVHSphere);
				extents.emplace_back(extent.first);
				extents.emplace_back(extent.second);
			}
			//Find index of the gameObj with that extent
			indexX = FindIndexWithExtents(objects, extents[(extents.size() - 1) / 2.f], startIndex, endIndex, 'x', renderBVHSphere);
			costX = GetHeuristicCost(objects, startIndex, indexX, numObjects);

			extents.clear();
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareY); //'Y' PLANE
			for (int i = startIndex; i <= endIndex; ++i)
			{
				std::pair<float, float> extent = BoundingVolume::getExtents(objects[i], 'y', renderBVHSphere);
				extents.emplace_back(extent.first);
				extents.emplace_back(extent.second);
			}
			indexY = FindIndexWithExtents(objects, extents[(extents.size() - 1) / 2.f], startIndex, endIndex, 'y', renderBVHSphere);
			costY = GetHeuristicCost(objects, startIndex, indexY, numObjects);

			extents.clear();
			std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareZ); //'Z' PLANE
			for (int i = startIndex; i <= endIndex; ++i)
			{
				std::pair<float, float> extent = BoundingVolume::getExtents(objects[i], 'z', renderBVHSphere);
				extents.emplace_back(extent.first);
				extents.emplace_back(extent.second);
			}
			indexZ = FindIndexWithExtents(objects, extents[(extents.size() - 1) / 2.f], startIndex, endIndex, 'z', renderBVHSphere);
			costZ = GetHeuristicCost(objects, startIndex, indexZ, numObjects);

			if (costX < costY && costX < costZ)
			{
				std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareX); //'X' PLANE
				return indexX;
			}
			else if (costY < costX && costY < costZ)
			{
				std::sort(std::begin(objects) + startIndex, std::begin(objects) + endIndex + 1, &compareY); //'Y' PLANE
				return indexY;
			}
			// else already sorted in z axis
			return indexZ;
		}
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

	int FindIndexClosestToPoint(std::vector<GameObject*>& objects, float point, int startIndex, int endIndex, char axis)
	{
		float minDist = FLT_MAX;
		int closestIndex = startIndex;
		for (int i = startIndex; i <= endIndex; ++i)
		{
			float dist = 0;
			if (axis == 'x')
				dist = abs(objects[i]->transform.Position.x - point);
			else if (axis == 'y')
				dist = abs(objects[i]->transform.Position.y - point);
			else
				dist = abs(objects[i]->transform.Position.z - point);
			if (dist < minDist)
			{
				minDist = dist;
				closestIndex = i;
			}
		}
		return closestIndex;
	}

	int FindIndexWithExtents(std::vector<GameObject*>& objects, float extent, int startIndex, int endIndex, char axis, bool renderSphere)
	{
		float minDist = FLT_MAX;
		int closestIndex = startIndex;
		for (int i = startIndex; i <= endIndex; ++i)
		{
			float dist = 0;
			if (renderSphere)
			{
				if (axis == 'x')
				{
					dist = std::min(abs(objects[i]->transform.Position.x + objects[i]->sphereBV.m_Position.x - objects[i]->sphereBV.m_Radius - extent),
						abs(objects[i]->transform.Position.x + objects[i]->sphereBV.m_Position.x + objects[i]->sphereBV.m_Radius - extent));
				}
				else if (axis == 'y')
				{
					dist = std::min(abs(objects[i]->transform.Position.y + objects[i]->sphereBV.m_Position.y - objects[i]->sphereBV.m_Radius - extent),
						abs(objects[i]->transform.Position.y + objects[i]->sphereBV.m_Position.y + objects[i]->sphereBV.m_Radius - extent));
				}
				else
				{
					dist = std::min(abs(objects[i]->transform.Position.z + objects[i]->sphereBV.m_Position.z - objects[i]->sphereBV.m_Radius - extent),
						abs(objects[i]->transform.Position.z + objects[i]->sphereBV.m_Position.z + objects[i]->sphereBV.m_Radius - extent));
				}
			}
			else
			{
				if (axis == 'x')
				{
					dist = std::min(abs(objects[i]->transform.Position.x - objects[i]->aabbBV.m_Min.x - extent), abs(objects[i]->transform.Position.x + objects[i]->aabbBV.m_Max.x - extent));
				}
				else if (axis == 'y')
				{
					dist = std::min(abs(objects[i]->transform.Position.y - objects[i]->aabbBV.m_Min.y - extent), abs(objects[i]->transform.Position.y + objects[i]->aabbBV.m_Max.y - extent));
				}
				else
				{
					dist = std::min(abs(objects[i]->transform.Position.z - objects[i]->aabbBV.m_Min.z - extent), abs(objects[i]->transform.Position.z + objects[i]->aabbBV.m_Max.z - extent));
				}
			}

			if (dist == 0.f)
			{
				return i; //return first object with that extent break out of loop
			}
		}
		return -1; //error the extents passed in do not belong to any of the objects
	}

	void BottomUpBVTree(Node** tree, std::vector<GameObject*>& objects)
	{
		int numObjects = objects.size();
		assert(numObjects != 0);
		int i, j;
		// Allocate temporary memory for holding node pointers
		//std::vector<Node*> pNodes{};
		Node** tempTree = new Node*[numObjects];
		// Form the leaf nodes for the given input objects
		for (i = 0; i < numObjects; i++) {
			tempTree[i] = new Node;
			tempTree[i]->type = Node::Type::LEAF;
			tempTree[i]->data = objects[i];
			tempTree[i]->pos = objects[i]->transform.Position;
			tempTree[i]->BV_AABB = objects[i]->aabbBV;
			tempTree[i]->BV_Sphere = objects[i]->sphereBV;
		}

		auto reverseDepth = 0;
		// Merge pairs together until just the root object left
		while (numObjects > 1) {
			// Find indices of the two "nearest" nodes, based on some criterion
			FindNodesToMerge(tempTree, numObjects, &i, &j);
			// Group nodes i and j together under a new internal node
			Node* pPair = new Node;
			pPair->type = Node::Type::INTERNAL;
			pPair->lChild = tempTree[i];
			pPair->rChild = tempTree[j];
			pPair->treeDepth = ++reverseDepth;
			// Compute a bounding volume for the two nodes
			if (renderBVHSphere)
			{
				pPair->BV_Sphere = ComputeBoundingSphere(objects, i, j);
			}
			else
			{
				pPair->BV_AABB = ComputeBoundingVolumeAABB(tempTree[i], tempTree[j]);
				pPair->pos = (pPair->BV_AABB.m_Min + pPair->BV_AABB.m_Max) * 0.5f; //centre of AABB
			}

			// Remove the two nodes from the active set and add in the new node.
			// Done by putting new node at index ’min’ and copying last entry to ’max’
			int min = i, max = j;
			if (i > j) min = j, max = i;
			tempTree[min] = pPair;
			tempTree[max] = tempTree[numObjects - 1];
			numObjects--;
		}
		// Free temporary storage and return root of tree
		Node* pRoot = tempTree[0];
		if (tempTree)
			delete tempTree; //just delete one dimension in the array
		*tree = pRoot;
	}

	void FindNodesToMerge(Node* nodes[], int numObjs, int* indexI, int* indexJ)
	{
		float minDist = FLT_MAX;

		//NEAREST NEIGHBOUR
		for (size_t i = 0; i < numObjs; ++i)
		{
			for (size_t j = 0; j < numObjs; ++j)
			{
				if (i == j) continue; //compare with every other node not itself
				float dist = 0;
				if (renderBVHSphere)
				{
					dist = glm::distance(nodes[i]->pos, nodes[j]->pos);
				}
				else
				{
					dist = glm::distance(nodes[i]->pos, nodes[j]->pos);
				}
				if (dist < minDist)
				{
					minDist = dist;
					*indexI = i;
					*indexJ = j;
				}
			}
		}
	}

	Collision::AABB ComputeBoundingVolumeAABB(Node* first, Node* second)
	{
		glm::vec3 Min, Max;

		Min.x = std::min(first->BV_AABB.m_Min.x,
			second->BV_AABB.m_Min.x);
		Max.x = std::max(first->BV_AABB.m_Max.x,
			second->BV_AABB.m_Max.x);

		Min.y = std::min(first->BV_AABB.m_Min.y,
			second->BV_AABB.m_Min.y);
		Max.y = std::max(first->BV_AABB.m_Max.y,
			second->BV_AABB.m_Max.y);

		Min.z = std::min(first->BV_AABB.m_Min.z,
			second->BV_AABB.m_Min.z);
		Max.z = std::max(first->BV_AABB.m_Max.z,
			second->BV_AABB.m_Max.z);

		return Collision::AABB(Min, Max);
	}

	//Collision::Sphere ComputeBoundingVolumeSphere(Node* first, Node* second)
	//{
	//	return Collision::Sphere();
	//}

	void SetBottomUpBVTreeDepth(Node* tree, int depth)
	{
		if (!tree)
			return;
		Node* node = tree;
		node->treeDepth = depth;
		SetBottomUpBVTreeDepth(node->lChild, depth + 1);
		SetBottomUpBVTreeDepth(node->rChild, depth + 1);
	}


}

