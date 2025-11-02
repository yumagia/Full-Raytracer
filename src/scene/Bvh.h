#ifndef BVH_INCLUDED
#define BVH_INCLUDED

#include "ScenePrimitives.h"

#include <vector>

struct BoundBoxf {
	void AddPoint(Vec3f p);

	Vec3f min, max;
};

// Size-optimized a bit
struct BvhNode {
	BoundBoxf bounds;

	uint left, right;

	uint firstTriangle, triangleCount;
};

class SceneBvh {
public:
	SceneBvh() {}
	~SceneBvh();

	SceneBvh(std::vector<Triangle> inputTriangles);

	void BuildBvh();
	void CalcBounds(uint nodeIdx);
	void Subdivide(uint nodeIdx);

	bool RayBvh(Vec3f start, Vec3f dir, const uint nodeIdx, float tMax, float &tHit, Triangle &triHit);

	std::vector<Triangle> triangles;
	uint rootIdx = 0;
	uint nodesUsed = 1;
	BvhNode *bvhNodes;
};

#endif