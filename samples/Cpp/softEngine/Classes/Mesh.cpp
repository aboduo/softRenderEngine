#include "Mesh.h"
#include "cocos2d.h"

#include "rapidjson/document.h"

#include <string>
using namespace std;
using namespace cocos2d;

void Mesh::loadFile(const char *fn) {
    rapidjson::Document d;
    unsigned long fsz;
    unsigned char *fd = CCFileUtils::sharedFileUtils()->getFileData(fn, "r", &fsz);
    string fcon = string((char*)fd, (size_t)fsz);
    CCLog("fcon %s", fcon.c_str());
    d.Parse<0>(fcon.c_str());
    
    CCLog("type %d", d["meshes"].GetType());
    int mn = d["meshes"].Size();
    rapidjson::GenericValue<rapidjson::UTF8<> > &meshes = d["meshes"][0u];
    CCLog("mn %d", mn);

    int len = meshes["vertices"].Size();
    float x, y, z;

    //跳过normal 数据
    for(int i=0; i < len; i++) {
        float fv = meshes["vertices"][i].GetDouble();
        if(i % 6 == 0) {
            x = fv;
        }else if(i % 6 == 1) {
            y = fv;
        } else if(i % 6 == 2) {
            z = fv;
            vertices.push_back({x, y, z});
        }
    }

    /*
    triangles.push_back({0, 2, 1});

    edges.push_back({0, 2});
    edges.push_back({2, 1});
    edges.push_back({1, 0});
    */

    int len2 = meshes["indices"].Size();
    int a, b, c;
    for(int i=0; i < len2; i++) {
        int idx = meshes["indices"][i].GetInt();
        if(i % 3 == 0) {
            a = idx;
        }else if(i % 3 == 1) {
            b = idx;
        } else {
            c = idx;
            triangles.push_back({a, b, c});

            edges.push_back({a, b});
            edges.push_back({b, c});
            edges.push_back({c, a});
        }
    }

    CCLog("vnum %d %d %d %d", len, len/3, len2, len2/3);
}



void Mesh::update(float diff) {
    passTime += diff;
    kmVec3 axis;
    kmVec3Fill(&axis, 0, 1, 0);
    kmQuaternion ry;
    kmQuaternionRotationAxis(&ry, &axis, passTime*3.14/2);

    kmQuaternion rx;
    kmVec3Fill(&axis, 1, 0, 0);
    kmQuaternionRotationAxis(&rx, &axis, passTime*3.14/2);

    kmQuaternionMultiply(&rotation, &rx, &ry);
}
