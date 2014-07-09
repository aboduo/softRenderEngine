#ifndef __LIGHT__
#define __LIGHT__
#include "kazmath/vec3.h"
#include "kazmath/vec2.h"
#include "kazmath/quaternion.h"
#include <vector>
#include "Mesh.h"

using namespace std;

//两种渲染方法 将对象传递给渲染器来 visit 访问顶点 边 面属性来渲染
class Light {

public:
    kmVec3 position;
    kmVec3 direction;
    vector<kmVec3> vertices;

    vector<Edge> edges;

    Light() {
        edges.push_back({0, 1});
    }

    void calculateEdge() {
        vertices.clear();
        vertices.push_back(position);
        kmVec3 temp;
        kmVec3Normalize(&direction, &direction);
        kmVec3Add(&temp, &position, &direction);
        vertices.push_back(temp);
    }

    void render() {
     
    }
};

#endif
