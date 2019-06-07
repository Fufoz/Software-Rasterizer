
//clipping stuff

#include "math.h"
#include <cassert>
namespace clipper{

#define MAX_CLIPPED_TRIANGLE_COUNT 4
#define MAX_CLIPPED_VERTEX_COUNT 6

struct Triangle
{
    Vec4 v1;
    Vec4 v2;
    Vec4 v3;
};

struct ClippResult
{
    Triangle triangles[MAX_CLIPPED_TRIANGLE_COUNT];
    size_t numTriangles;//num vertices - 2
};

//if all bits are zero then the point is inside the view frustum
enum PlaneBits
{
    PLANE_LEFT_BIT   = 1 << 0,
    PLANE_RIGHT_BIT  = 1 << 1,
    PLANE_TOP_BIT    = 1 << 2,
    PLANE_BOTTOM_BIT = 1 << 3,
    PLANE_NEAR_BIT   = 1 << 4,
    PLANE_FAR_BIT    = 1 << 5
};

struct VertexBuffer
{
    size_t size;
    Vec4 clippedVertices[MAX_CLIPPED_VERTEX_COUNT];
};

static void vbPushData(VertexBuffer& vbuffer, const Vec4& data)
{
    assert(vbuffer.size < MAX_CLIPPED_VERTEX_COUNT);
    vbuffer.clippedVertices[vbuffer.size] = data;
    vbuffer.size++;
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
    }
}

static Vec4 intersectPlaneSegment(const Vec4& v1, const Vec4& v2, PlaneBits plane)
{

    float sign = 0.f;
    float val1 = 0.f;
    float val2 = 0.f;

    switch(plane) {
        case PLANE_LEFT_BIT : 
        case PLANE_RIGHT_BIT :
            val1 = v1.x; val2 = v2.x;
            break;
        case PLANE_TOP_BIT :
        case PLANE_BOTTOM_BIT:
            val1 = v1.y; val2 = v2.y;
            break;
        case PLANE_FAR_BIT :
        case PLANE_NEAR_BIT :
            val1 = v1.z; val2 = v2.z;
            break;
    }

    if(plane & (PLANE_LEFT_BIT|PLANE_BOTTOM_BIT|PLANE_NEAR_BIT))
        sign = 1.f;
    else{
        sign = -1.f;
    }

    float amount = (v1.w + val1 * sign) / (v1.w  + val1 * sign - v2.w  - val2 * sign);

    return lerp(v1, v2, amount);
}

static VertexBuffer clipAgainstEdge(const VertexBuffer& in, PlaneBits clipPlane)
{
    //setting starting point equals to last point in input array
    Vec4 startPoint = in.clippedVertices[in.size - 1];
    
    //resetting output buffer
    VertexBuffer out = {};

    for(int i = 0; i < in.size; i++) {
        Vec4 endPoint = in.clippedVertices[i];
        if(isVertexInsidePlane(startPoint, clipPlane)) {
            if(isVertexInsidePlane(endPoint, clipPlane)) {
                //printf("CLIPPER: IN_IN\n");
                vbPushData(out, endPoint);//IN_IN
            } else {
            //IN_OUT
                //printf("CLIPPER: IN_OUT\n");
                //push straddled point
                Vec4 straddledPoint =  intersectPlaneSegment(startPoint, endPoint, clipPlane);
                vbPushData(out, straddledPoint);
            }
        }
        else{
            if(isVertexInsidePlane(endPoint, clipPlane)) { //OUT_IN
                //printf("CLIPPER: OUT_IN\n"); 
                //push straddled point
                Vec4 straddledPoint =  intersectPlaneSegment(startPoint, endPoint, clipPlane);
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

ClippResult clipTriangle(const Vec4& v1, const Vec4& v2, const Vec4& v3)
{
    ClippResult result = {};
    VertexBuffer in = {3, {v1, v2, v3}};
    VertexBuffer out = {};

    out = clipAgainstEdge(in, PLANE_LEFT_BIT);
    in = clipAgainstEdge(out, PLANE_RIGHT_BIT);
    out = clipAgainstEdge(in, PLANE_TOP_BIT);
    in = clipAgainstEdge(out, PLANE_BOTTOM_BIT);
    out = clipAgainstEdge(in, PLANE_NEAR_BIT);
    in = clipAgainstEdge(out, PLANE_FAR_BIT);
    
    if(in.size == 0)
        return result;
    
    result.numTriangles = in.size - 2;
    Vec4 refPoint = in.clippedVertices[0];

    for(int i = 0; i < result.numTriangles; i++) {
        Triangle& tr = result.triangles[i];
        tr.v1 = refPoint;
        tr.v2 = in.clippedVertices[i + 1]; 
        tr.v3 = in.clippedVertices[i + 2];
 //       printf("Triangle: v1 : {%f, %f, %f} v2:{%f, %f, %f} v3:{%f, %f, %f}\n",
 //       tr.v1.x,tr.v1.y,tr.v1.z,
 //       tr.v2.x,tr.v2.y,tr.v2.z,
 //       tr.v3.x,tr.v3.y,tr.v3.z);
    }

    return result;
}


}//!cliper