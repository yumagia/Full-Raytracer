#include "Bvh.h"
#include "Raytracer.h"

#include <vector>
#include <iostream>

#define GIANT_NUM 1e20f

SceneBvh::SceneBvh(std::vector<Triangle> inputTriangles) {
	numTris = inputTriangles.size();
	if(numTris == 0) {
		return;
	}
	triangles = new Triangle[numTris];
	int i = 0;

	for(Triangle triangle : inputTriangles) {
		triangles[i] = triangle;
		i++;
	}

	bvhNodes = new BvhNode[numTris * 2]; // Allocate an array of bvhNodes to store the tree
}

SceneBvh::~SceneBvh() {
	delete[] bvhNodes;
	delete[] triangles;
}

bool SceneBvh::BuildBvh() {

	if(numTris == 0) {
		return false;
	}
	BvhNode &root = bvhNodes[rootIdx];
	root.left = 0;
	root.firstTriangle = 0;
	root.triangleCount = numTris;

	CalcBounds(rootIdx);
	Subdivide(rootIdx);

	return true;
}

void BoundBoxf::AddPoint(Vec3f p) {
	if(p.x < min.x) {
		min.x = p.x;
	}
	if(p.y < min.y) {
		min.y = p.y;
	}
	if(p.z < min.z) {
		min.z = p.z;
	}
	if(p.x > max.x) {
		max.x = p.x;
	}
	if(p.y > max.y) {
		max.y = p.y;
	}
	if(p.z > max.z) {
		max.z = p.z;
	}
}

void SceneBvh::CalcBounds(uint nodeIdx) {
	BvhNode &node = bvhNodes[nodeIdx];
	node.bounds.min = Vec3f(GIANT_NUM, GIANT_NUM, GIANT_NUM);
	node.bounds.max = Vec3f(-GIANT_NUM, -GIANT_NUM, -GIANT_NUM);
	uint first = node.firstTriangle;
	for(int i = 0; i < node.triangleCount; i++) {
		Triangle &triangle = triangles[first + i];
		node.bounds.AddPoint(triangle.v1);
		node.bounds.AddPoint(triangle.v2);
		node.bounds.AddPoint(triangle.v3);
	}
}

void SceneBvh::Subdivide(uint nodeIdx) {
	BvhNode &node = bvhNodes[nodeIdx];

	if(node.triangleCount <= 2) {		// Stop when there are at least two faces (you often can't split further)
		return;
	}

	// Find greatest axis and split down the center
	Vec3f extent = node.bounds.max - node.bounds.min;
	float splitPos;
	int axis = 0;
	if(extent.y > extent.x && extent.y > extent.z) {
		axis = 1;
		splitPos = node.bounds.min.y + extent.y * 0.5f;
	}
	else if(extent.z > extent.x && extent.z > extent.y) {
		axis = 2;
		splitPos = node.bounds.min.z + extent.z * 0.5f;
	}
	else {
		splitPos = node.bounds.min.x + extent.x * 0.5f;
	}
	
	// Lomuto partition this
	int i = node.firstTriangle;
	int j = i + node.triangleCount - 1;

	while(i <= j) {
		Triangle triangle = triangles[i];
		float centroidAxis;
		if(axis = 0) {
			centroidAxis = (triangle.v1.x + triangle.v1.x + triangle.v1.x) / 3;
		}
		else if(axis = 1) {
			centroidAxis = (triangle.v1.y + triangle.v1.y + triangle.v1.y) / 3;
		}
		else if(axis = 2) {
			centroidAxis = (triangle.v1.z + triangle.v1.z + triangle.v1.z) / 3;
		}

		if(centroidAxis < splitPos) {
			i++;
		}
		else {
			triangles[i] = triangles[j];
			triangles[j--] = triangle;
		}
		
	};
	
	int leftCount = i - node.firstTriangle;
	if(leftCount == 0 || leftCount == node.triangleCount) {	// Rare empty box?
		return;
	}

	// Create child nodes
	int leftIdx = nodesUsed++;
	int rightIdx = nodesUsed++;
	bvhNodes[leftIdx].firstTriangle = node.firstTriangle;
	bvhNodes[leftIdx].triangleCount = leftCount;
	bvhNodes[rightIdx].firstTriangle = i;
	bvhNodes[rightIdx].triangleCount = node.triangleCount - leftCount;
	node.left = leftIdx;
	node.triangleCount = 0;
	CalcBounds(leftIdx);
	CalcBounds(rightIdx);

	// Recurse
	Subdivide(leftIdx);
	Subdivide(rightIdx);
}

// Slab method hit check
// This does not return a hit point
bool HitCheckBoundingBox(Vec3f start, Vec3f dir, BoundBoxf boundingBox) {
	float tMin = (boundingBox.min.x - start.x) / dir.x;
	float tMax = (boundingBox.max.x - start.x) / dir.x;
	if(tMin > tMax) {
		float temp  = tMin;
		tMin = tMax;
		tMax  = temp;
	}

	float tyMin = (boundingBox.min.y - start.y) / dir.y;
	float tyMax = (boundingBox.max.y - start.y) / dir.y;
	if(tyMin > tyMax) {
		float temp  = tyMin;
		tyMin = tyMax;
		tyMax = temp;
	}

	if((tMin > tyMax) || (tyMin > tMax)) {
		return false;
	}

	if(tyMin > tMin) {
		tMin = tyMin;
	}
	if(tyMax < tMax) {
		tMax = tyMax;
	}

	float tzMin = (boundingBox.min.z - start.z) / dir.z;
	float tzMax = (boundingBox.max.z - start.z) / dir.z;
	if(tzMin > tzMax) {
		float temp  = tzMin;
		tzMin = tzMax;
		tzMax = temp;
	}

	return !((tMin > tzMax) || (tzMin > tMax));
}

// Recursive hit routine on Bvh
bool SceneBvh::RayBvh(Vec3f start, Vec3f dir, const uint nodeIdx, float tMax, float &tHit, Triangle &triHit) {
	bool hit = false;
	BvhNode &node = bvhNodes[nodeIdx];
	if(!HitCheckBoundingBox(start, dir, node.bounds)) {		// Missed
		return false;
	}
	if(node.triangleCount > 0) {		// In a leaf		
		for(int i = 0; i < node.triangleCount; i++) {
			if(HitCheckTriangle(start, dir, triangles[node.firstTriangle + i], tMax, tHit)) {
				triHit = triangles[node.firstTriangle + i];
				hit = true;
				tMax = tHit;
			}
		}
	}
	else {
		hit |= RayBvh(start, dir, node.left, tMax, tHit, triHit);
		hit |= RayBvh(start, dir, node.left + 1, tMax, tHit, triHit);
	}

	return hit;
}

