#pragma once
#include "GameObject.h"
#include "BoundingVolume.h"
#include "Collision.h"

namespace BVHierarchy
{
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
		glm::vec3 pos{}; //used only for bottom up for now

		Node* lChild{}, * rChild{}; // Child node pointers
	};
	char FindLargestAxis(std::vector<GameObject*>& objects);

	Collision::AABB ComputeBoundingVolume(std::vector<GameObject*>& objects, int startIndex, int numObjects);
	Collision::Sphere ComputeBoundingSphere(std::vector<GameObject*>& objects, int startIndex, int numObjects);
	// Construct a top-down tree. Rearranges object[] array during construction
	void TopDownBVTree(Node** tree, std::vector<GameObject*>& objects, int startIndex, int endIndex, int depth);
	
	float GetHeuristicCost(std::vector<GameObject*>& objects, int startIndex, int split, int endIndex);
	int PartitionObjects(std::vector<GameObject*>& objects, int startIndex, int endIndex);

	Collision::Sphere RecomputeParentSphere(Collision::Sphere& parent, Collision::Sphere& lchild, Collision::Sphere& rchild);

	int FindIndexClosestToPoint(std::vector<GameObject*>& objects, float point, int startIndex, int endIndex, char axis);

	int FindIndexWithExtents(std::vector<GameObject*>& objects, float extent, int startIndex, int endIndex, char axis, bool renderSphere);

	void BottomUpBVTree(Node** tree, std::vector<GameObject*>& objects);

	void FindNodesToMerge(Node* nodes[], int numObjs, int* indexI, int* indexJ);

	Collision::AABB BtmUpComputeBoundingVolumeAABB(Node* first, Node* second);

	Collision::Sphere BtmUpComputeBoundingVolumeSphere(Node* first, Node* second);

	void SetBottomUpBVTreeDepth(Node* tree, int depth);

	float NearestNeighbourCost(Node* nodes[], int i, int j); //returns a lower cost if nearer distance

	float CombinedVolumeCost(Node* nodes[], int i, int j); //returns a lower cost if smallest volume (volume proportional to SA)

	float RelativeIncreaseCost(Node* nodes[], int i, int j); //returns a lower cost if smallest relative increase

	enum class Tree
	{
		TOP_DOWN_MEDIAN_SPLIT,
		TOP_DOWN_K_EVEN_SPLIT,
		TOP_DOWN_MEDIAN_EXTENTS_SPLIT
	};
}

