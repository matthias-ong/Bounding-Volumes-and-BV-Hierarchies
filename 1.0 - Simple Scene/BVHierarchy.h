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
	char FindLargestAxis(GameObject* objects[], int numObjects);

	Collision::AABB ComputeBoundingVolume(GameObject* objects[], int numObjects);
	// Construct a top-down tree. Rearranges object[] array during construction
	void TopDownBVTree(Node** tree, GameObject* objects[], int numObjects, int depth);
	
	float GetHeuristicCost(GameObject* objects[], int startIndex, int endIndex);
	int PartitionObjects(GameObject* objects[], int numObjects);

	//void RenderTopDownBVTree(Node** tree);
}

