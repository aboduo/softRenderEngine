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

    float nx, ny, nz;
    float tx, ty;

    //跳过normal 数据
    int modV = 8;
    for(int i=0; i < len; i++) {
        float fv = meshes["vertices"][i].GetDouble();
        if(i % modV == 0) {
            x = fv;
        }else if(i % modV == 1) {
            y = fv;
        } else if(i % modV == 2) {
            z = fv;
            vertices.push_back({x, y, z});
        } else if(i % modV == 3) {
            nx = fv;
        } else if(i % modV == 4) {
            ny = fv;
        } else if(i % modV == 5) {
            nz = fv;
            normal.push_back({nx, ny, nz});
        } else if(i % modV == 6 ) {
            tx = fv;
        } else if(i % modV == 7) {
            ty = fv;
            textureCoord.push_back({tx, ty});
        }
    }
    for(int i = 0; i <vertices.size(); i++) {
        CCLog("vex %f %f %f", vertices[i].x, vertices[i].y, vertices[i].z);
        CCLog("nor %f %f %f", normal[i].x, normal[i].y, normal[i].z);
        CCLog("tex %f %f", textureCoord[i].x, textureCoord[i].y);
    }

    /*
    int lend = 1;
    //int ids[] = {0, 1, 2, 3, 4, 5, 0, 6, 7, 1, 8, 9};
    int ids[] = {1, 8, 9};

    for(int i = 0; i < lend; i++) {
        triangles.push_back({ids[i*3], ids[i*3+1], ids[i*3+2]});
        edges.push_back({ids[i*3], ids[i*3+1]});
        edges.push_back({ids[i*3+1], ids[i*3+2]});
        edges.push_back({ids[i*3+2], ids[i*3]});
    }
    */
    
    //triangles.push_back({0, 1, 2});
    //triangles.push_back({0, 1, 7});
    //triangles.push_back({3, 7, 4});

    //triangles.push_back({0, 3, 2});
    //triangles.push_back({3, 5, 2});



    //edges.push_back({0, 1});

    //edges.push_back({3, 7});
    //edges.push_back({7, 4});
    //edges.push_back({4, 3});

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
            CCLog("tra %d %d %d", a, b, c);
            triangles.push_back({a, b, c});

            edges.push_back({a, b});
            edges.push_back({b, c});
            edges.push_back({c, a});
        }
    
    }

    CCLog("vnum %d %d %d %d", len, len/3, len2, len2/3);

}



void Mesh::update(float diff) {
    //return;

    passTime += diff;
    kmVec3 axis;
    kmVec3Fill(&axis, 0, 1, 0);
    kmQuaternion ry;

    float t = 3;
    kmQuaternionRotationAxis(&rotation, &axis, -passTime*3.14/t);
    
    //kmQuaternionRotationAxis(&ry, &axis, passTime*3.14/t);

    /*
    kmQuaternion rx;
    kmVec3Fill(&axis, 1, 0, 0);
    kmQuaternionRotationAxis(&rx, &axis, passTime*3.14/t);
    */

    //kmQuaternionMultiply(&rotation, &rx, &ry);
}
