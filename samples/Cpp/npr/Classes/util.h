#ifndef __UTIL__
#define __UTIL__
#include "cocos2d.h"
#include "kazmath/vec4.h"
using namespace cocos2d;

void drawLine(unsigned char *data, int width, int height, int x1, int y1, int x2, int y2, int c);
void drawTriangle(unsigned char *data, float *depth, int width, int height, int x1, int y1, float z1, int x2, int y2, float z2,  int x3, int y3, float z3,  int col);
void drawFace(unsigned char *data, float *depth, int width, int height, int x1, int y1, float z1, int x2, int y2, float z2,  int x3, int y3, float z3,  int col);

void drawFaceWithLight(unsigned char *data, float *depth, int width, int height, int x1, int y1, float z1, kmVec3 col1, int x2, int y2, float z2, kmVec3 col2,  int x3, int y3, float z3,  kmVec3 col3);


//渲染从光源点 看到的 面深度 到 lightDepth 中
void drawLightDepth(float *lightDepth, int width, int height, int x1, int y1, float z1, int x2, int y2, float z2,  int x3, int y3, float z3);


void drawShadow(unsigned char *data, float *depth, float *lightDepth, int width, int height, kmMat4 lightMatrix, kmVec3 p1, kmVec3 wp1, kmVec3 p2, kmVec3 wp2, kmVec3 p3, kmVec3 wp3);

struct PosTexDiff {
    kmVec3 p;
    kmVec2 tex;
    float diff;
};

void drawFaceNPR(unsigned char *data, float *depth, unsigned char *texture, int width, int height, int imgWidth, int imgHeight, unsigned char *nprShade, CCSize nprSize, PosTexDiff p1,  PosTexDiff p2, PosTexDiff p3);


struct PosTexNor {
    kmVec3 p;
    kmVec2 tex;
    kmVec3 normal;
};


//light direction 1 1 1
//normal 标记每个定点的法向量 计算diff
void drawFaceNPRPerPixel(unsigned char *data, float *depth, unsigned char *texture, int width, int height, int imgWidth, int imgHeight, unsigned char *nprShade, CCSize nprSize, kmVec3 lightDir, PosTexNor p1,  PosTexNor p2, PosTexNor p3);



void drawFaceWithTexture(unsigned char *data, float *depth, unsigned char *texture, int width, int height, int imgWidth, int imgHeight, int x1, int y1, float z1, kmVec2 tex1, int x2, int y2, float z2, kmVec2 tex2,  int x3, int y3, float z3,  kmVec2 tex3);
#endif

