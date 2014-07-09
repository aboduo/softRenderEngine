#ifndef __NORMAL_MAP__
#define __NORMAL_MAP____
#include "kazmath/vec3.h"
#include "kazmath/vec2.h"
#include "kazmath/quaternion.h"
#include <vector>
#include "Mesh.h"


using namespace std;

//两种渲染方法 将对象传递给渲染器来 visit 访问顶点 边 面属性来渲染
//根据计算的normalMap float 数值
//还原一定数量的 向量 line
class NormalMap {

public:
    int width;
    int height;

    //设置centerPos 中的值
    //和normal的值
    vector<kmVec4> centerPos;
    vector<kmVec3> normalMap;

    vector<kmVec4> vertices;
    vector<Edge> edges;


    NormalMap() {
    }

    //传入每个 定点的位置
    //传入每个定点的方向
    //position map
    void calculateEdge() {
        edges.clear();
        vertices.clear();
        
        //w 向量
        for(int i=0; i < centerPos.size(); i++) {
            kmVec4 vt = centerPos[i];
            vt.x /= vt.w;
            vt.y /= vt.w;
            vt.z /= vt.w;
            vt.w = 1;

            kmVec3 newpos;
            kmVec3Fill(&newpos, vt.x, vt.y, vt.z);

            kmVec3 dir = normalMap[i];
            kmVec3 temp;
            kmVec3Normalize(&dir, &dir);
            kmVec3Scale(&dir, &dir, 0.2);
            kmVec3Add(&temp, &newpos, &dir);


            vertices.push_back(vt);
            vertices.push_back({temp.x, temp.y, temp.z, 1});

            int len = vertices.size();
            edges.push_back({len-2, len-1});    
        }
        CCLog("edge %d %d", vertices.size(), edges.size());
    }

    void render() {
     
    }
};

#endif
