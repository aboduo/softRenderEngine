#ifndef __MESH__
#define __MESH__
#include "kazmath/vec3.h"
#include "kazmath/quaternion.h"
#include <vector>


//画线 drawLine 等粗细 没有远近
struct Edge {
    int a;
    int b;
};

    
struct Triangle {
    int a;
    int b;
    int c;
};

//
class Mesh {
public:
    std::vector<kmVec3> vertices;
    std::vector<kmVec3> normal;
    std::vector<kmVec2> textureCoord;

    std::vector<Edge> edges;
    //int 1 b
    std::vector<Triangle> triangles;



    kmVec3 position;
    //kmVec3 rotation;
    kmQuaternion rotation;


    void update(float diff);

    float passTime;
    Mesh():
    passTime(0)
    {
    }

    void loadFile(const char* fn);

};

#endif
