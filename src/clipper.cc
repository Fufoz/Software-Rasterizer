
//clipping stuff
#include "clipper.h"
#include <cassert>

enum PlaneBits
{
	PLANE_LEFT_BIT   = 1 << 0,
	PLANE_RIGHT_BIT  = 1 << 1,
	PLANE_TOP_BIT    = 1 << 2,
	PLANE_BOTTOM_BIT = 1 << 3,
	PLANE_NEAR_BIT   = 1 << 4,
	PLANE_FAR_BIT    = 1 << 5
};
const uint8_t PLANE_COUNT = 6;

struct VertexBuffer
{
	size_t size;
	Vertex clippedVertices[MAX_CLIPPED_VERTEX_COUNT];
};

static void vbPushData(VertexBuffer& vbuffer, const Vertex& data)
{
	assert(vbuffer.size < MAX_CLIPPED_VERTEX_COUNT);
	vbuffer.clippedVertices[vbuffer.size] = data;
	vbuffer.size++;
}

bool isInsideViewFrustum(const Vec4& pos)
{
	return pos.x <= pos.w && pos.y <= pos.w && pos.z <= pos.w
		&& pos.x >= -pos.w && pos.y >= -pos.w && pos.z >= -pos.w;
}

static inline bool isVertexInsidePlane(const Vec4& vertex, PlaneBits plane)
{
	switch(plane) {
		case PLANE_LEFT_BIT : return vertex.x >= -vertex.w; 
		case PLANE_RIGHT_BIT : return vertex.x <= vertex.w;
		case PLANE_TOP_BIT : return vertex.y <= vertex.w;
		case PLANE_BOTTOM_BIT : return vertex.y >= -vertex.w;
		case PLANE_NEAR_BIT : return vertex.z >= -vertex.w;
		case PLANE_FAR_BIT : return vertex.z <= vertex.w;
		default : {
			return false;
		}
	}
}

static Vertex intersectPlaneSegment(const Vertex& v1, const Vertex& v2, PlaneBits plane)
{

	float sign = 0.f;
	float val1 = 0.f;
	float val2 = 0.f;

	switch(plane) {
		case PLANE_LEFT_BIT : 
		case PLANE_RIGHT_BIT :
			val1 = v1.pos.x; val2 = v2.pos.x;
			break;
		case PLANE_TOP_BIT :
		case PLANE_BOTTOM_BIT:
			val1 = v1.pos.y; val2 = v2.pos.y;
			break;
		case PLANE_FAR_BIT :
		case PLANE_NEAR_BIT :
			val1 = v1.pos.z; val2 = v2.pos.z;
			break;
	}

	if(plane & (PLANE_LEFT_BIT|PLANE_BOTTOM_BIT|PLANE_NEAR_BIT))
		sign = 1.f;
	else{
		sign = -1.f;
	}

	float amount = (v1.pos.w + val1 * sign) / (v1.pos.w  + val1 * sign - v2.pos.w  - val2 * sign);

	Vertex straddledVertex = {};
	straddledVertex.pos = lerp(v1.pos, v2.pos, amount);
	straddledVertex.texCoords = lerp(v1.texCoords, v2.texCoords, amount);
	straddledVertex.color = lerp(v1.color, v2.color, amount);
	straddledVertex.normal = lerp(v1.normal, v2.normal, amount);
	straddledVertex.tangent = lerp(v1.tangent, v2.tangent, amount);
	return straddledVertex;
}

static VertexBuffer clipAgainstEdge(const VertexBuffer& in, PlaneBits clipPlane)
{
	//setting starting point equals to last point in input array
	Vertex startPoint = in.clippedVertices[in.size - 1];
	
	//resetting output buffer
	VertexBuffer out = {};

	for(size_t i = 0; i < in.size; i++) {
		Vertex endPoint = in.clippedVertices[i];
		if(isVertexInsidePlane(startPoint.pos, clipPlane)) {
			if(isVertexInsidePlane(endPoint.pos, clipPlane)) {
				//printf("CLIPPER: IN_IN\n");
				vbPushData(out, endPoint);//IN_IN
			} else {
				//IN_OUT
				//printf("CLIPPER: IN_OUT\n");
				//push straddled point
				Vertex straddledPoint = intersectPlaneSegment(startPoint, endPoint, clipPlane);
				vbPushData(out, straddledPoint);
			}
		}
		else{
			if(isVertexInsidePlane(endPoint.pos, clipPlane)) { //OUT_IN
				//printf("CLIPPER: OUT_IN\n"); 
				//push straddled point
				Vertex straddledPoint = intersectPlaneSegment(startPoint, endPoint, clipPlane);
				vbPushData(out, straddledPoint);
				//push end Point
				vbPushData(out, endPoint);
			}else{
				//printf("CLIPPER: OUT_OUT\n");
				//OUT_OUT
				//do nothing
			}
		}

		startPoint = endPoint;
	}

	return out;
}

ClippResult clipTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3)
{
	ClippResult result = {};
	VertexBuffer in = {3, {v1, v2, v3}};
	VertexBuffer out = {};

	int currentPlane = PLANE_LEFT_BIT;
	for(int i = 0; i < PLANE_COUNT; i++) {
		out = clipAgainstEdge(in, (PlaneBits)currentPlane);
		currentPlane <<= 1;
		in = out;
	}

	if(in.size == 0)
		return result;
	
	result.numTriangles = in.size - 2;

	Vec4 refPoint = in.clippedVertices[0].pos;
	Vec3 refT = in.clippedVertices[0].texCoords;
	Vec3 refC = in.clippedVertices[0].color;
	Vec3 refN = in.clippedVertices[0].normal;
	Vec3 refTan = in.clippedVertices[0].tangent;

	for(size_t i = 0; i < result.numTriangles; i++) {
		Triangle& tr = result.triangles[i];
		tr.v1.pos = refPoint;
		tr.v1.texCoords = refT;
		tr.v1.color = refC;
		tr.v1.normal = refN;
		tr.v1.tangent = refTan;

		tr.v2.pos = in.clippedVertices[i + 1].pos; 
		tr.v3.pos = in.clippedVertices[i + 2].pos;

		tr.v2.texCoords = in.clippedVertices[i + 1].texCoords; 
		tr.v3.texCoords = in.clippedVertices[i + 2].texCoords;

		tr.v2.color = in.clippedVertices[i + 1].color; 
		tr.v3.color = in.clippedVertices[i + 2].color;

		tr.v2.normal = in.clippedVertices[i + 1].normal; 
		tr.v3.normal = in.clippedVertices[i + 2].normal;

		tr.v2.tangent = in.clippedVertices[i + 1].tangent; 
		tr.v3.tangent = in.clippedVertices[i + 2].tangent;

 //       printf("Triangle: v1 : {%f, %f, %f} v2:{%f, %f, %f} v3:{%f, %f, %f}\n",
 //       tr.v1.x,tr.v1.y,tr.v1.z,
 //       tr.v2.x,tr.v2.y,tr.v2.z,
 //       tr.v3.x,tr.v3.y,tr.v3.z);
	}

	return result;
}

