
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
    PLANE_FAR_BIT    = 1 << 5,
};
typedef int VertexCode;
static const int PLANES_COUNT = 6;

enum EdgeOrientBits
{
    EDGE_IN_IN_BIT,
    EDGE_OUT_OUT_BIT,
    EDGE_IN_OUT_BIT,
    EDGE_OUT_IN_BIT
};

static Vec4 LEFT   {1.f, 0.f, 0.f, 1.f};
static Vec4 RIGHT  {-1.f, 0.f, 0.f, 1.f};
static Vec4 TOP    {0.f, -1.f, 0.f, 1.f};
static Vec4 BOTTOM {0.f, 1.f, 0.f, 1.f};
static Vec4 NEAR   {0.f, 0.f, 1.f, 1.f};
static Vec4 FAR    {0.f, 0.f, -1.f, 1.f};

union BoundaryCoord
{
    struct { 
        float left;
        float right;
        float top;
        float bottom;
        float near;
        float far;
    };

    float planes[6];
};

struct ClipVertex
{
    Vec4 pos;
    BoundaryCoord bc;
    VertexCode code;
};

struct VertexBuffer
{
    size_t currPos;
    Vec4 clippedVertices[MAX_CLIPPED_VERTEX_COUNT];
};

static BoundaryCoord computeBoundaryCoordinates(const Vec4& vertex)
{
    BoundaryCoord values = {};
    values.left   = dotVec4(vertex, LEFT);
    values.right  = dotVec4(vertex, RIGHT);
    values.top    = dotVec4(vertex, TOP);
    values.bottom = dotVec4(vertex, BOTTOM);
    values.near   = dotVec4(vertex, NEAR);
    values.far    = dotVec4(vertex, FAR);
    return values;
}

static VertexCode computePlaneMask(const BoundaryCoord& bc)
{   
    VertexCode code = {};

    if(bc.left < 0)
        code |= PLANE_LEFT_BIT;
    if(bc.right < 0)
        code |= PLANE_RIGHT_BIT;
    if(bc.top < 0)
        code |= PLANE_TOP_BIT;
    if(bc.bottom < 0)
        code |= PLANE_BOTTOM_BIT;
    if(bc.near < 0)
        code |= PLANE_NEAR_BIT;
    if(bc.far < 0)
        code |= PLANE_FAR_BIT;

    return code;
}


static void vbPushData(VertexBuffer& vbuffer, const Vec4& data)
{
    assert(vbuffer.currPos < MAX_CLIPPED_VERTEX_COUNT);
    vbuffer.clippedVertices[vbuffer.currPos] = data;
    vbuffer.currPos++;
}

static void clipEdge(const ClipVertex& begin, const ClipVertex& end, VertexBuffer& out)
{
    if((begin.code | end.code) == 0) {
        //trivial accept
        vbPushData(out, begin.pos);
        vbPushData(out, end.pos);
    } else if( begin.code & end.code) {
        //trivial reject. both points share the same outside plane
    } else { //nontrivial stuff
        VertexCode allPlanes = begin.code | end.code;
        VertexCode planeIterator = 1;
        float alpha0 = 0.f;
        float alpha1 = 1.f;

        //process single edge
        for(int i = 0; i < PLANES_COUNT; i++) {
            
            if(allPlanes & planeIterator) {
                float alpha = begin.bc.planes[planeIterator] / (begin.bc.planes[planeIterator] - end.bc.planes[planeIterator]);
                if(begin.code & allPlanes) {
                    //we're basically narrowing down line segment from each side
                    alpha0 = max(alpha0, alpha);
                }else{
                    alpha1 = min(alpha1, alpha);
                }
            }

            if(alpha1 < alpha0) // non-trivial reject
                return;
            planeIterator <<= 1;
        }

        Vec4 beginPos = begin.pos;
        Vec4 endPos = end.pos;

        if(begin.code) {
            beginPos = begin.pos + alpha0 * (end.pos - begin.pos);
        }

        if(end.code) {
            endPos = begin.pos + alpha1 * (end.pos - begin.pos);
        }

        vbPushData(out, beginPos);
        vbPushData(out, endPos);
    }


}

ClippResult clipTriangle(const Vec4& v1, const Vec4& v2, const Vec4& v3)
{
    ClipVertex data1 = {};
    ClipVertex data2 = {};
    ClipVertex data3 = {};
    
    data1.pos = v1;
    data2.pos = v2;
    data3.pos = v3;
    data1.bc = computeBoundaryCoordinates(v1);
    data2.bc = computeBoundaryCoordinates(v2);
    data3.bc = computeBoundaryCoordinates(v3);    
    data1.code = computePlaneMask(data1.bc);
    data2.code = computePlaneMask(data2.bc);
    data3.code = computePlaneMask(data3.bc);

    VertexBuffer vBuff = {};
    clipEdge(data1, data2, vBuff);
    clipEdge(data2, data3, vBuff);
    clipEdge(data3, data1, vBuff);

    ClippResult result = {};

    //incrorrect!    
    result.numTriangles = vBuff.currPos / 2 - 2;

    if(result.numTriangles > 0) {
        Vec4 referenceVert = vBuff.clippedVertices[0];
        for(size_t i = 0; i < result.numTriangles; i++) {
            Triangle& tr = result.triangles[i];
            tr.v1 = referenceVert;
            tr.v2 = vBuff.clippedVertices[i + 1];
            tr.v3 = vBuff.clippedVertices[i + 3];
        }
    }
    return result;
}


}//!cliper