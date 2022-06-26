#pragma once
#include "GameObject.h"
#include "BoundingVolume.h"
#include "Collision.h"

namespace BVHierarchy
{
	//GameObject* BVHObjs[4];
	struct Node
	{
		enum class Type
		{
			INTERNAL,
			LEAF
		};
		Collision::AABB BV_AABB{ {0.f, 0.f,0.f}, {0.f, 0.f, 0.f} };
		Collision::Sphere BV_Sphere{ {0.f, 0.f,0.f} , 0.f};
		Node::Type type{};
		GameObject* data{}; // pointer to objects/BVs that the node represents
		int numObjects{}; // How many objects in this subtree?
		int treeDepth = 0;

		Node* lChild{}, * rChild{}; // Child node pointers
	};
	char FindLargestAxis(std::vector<GameObject*>& objects);

	Collision::AABB ComputeBoundingVolume(std::vector<GameObject*>& objects, int startIndex, int numObjects);
	// Construct a top-down tree. Rearranges object[] array during construction
	void TopDownBVTree(Node** tree, std::vector<GameObject*>& objects, int startIndex, int endIndex, int depth);
	
	float GetHeuristicCost(std::vector<GameObject*>& objects, int startIndex, int split, int endIndex);
	int PartitionObjects(std::vector<GameObject*>& objects, int startIndex, int endIndex);

	//void RenderTopDownBVTree(Node** tree);
}

