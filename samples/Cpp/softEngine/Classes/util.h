#ifndef __UTIL__
#define __UTIL__
#include "cocos2d.h"
void drawLine(unsigned char *data, int width, int height, int x1, int y1, int x2, int y2, int c);
void drawTriangle(unsigned char *data, float *depth, int width, int height, int x1, int y1, float z1, int x2, int y2, float z2,  int x3, int y3, float z3,  int col);
void drawFace(unsigned char *data, float *depth, int width, int height, int x1, int y1, float z1, int x2, int y2, float z2,  int x3, int y3, float z3,  int col);

void drawFaceWithLight(unsigned char *data, float *depth, int width, int height, int x1, int y1, float z1, kmVec3 col1, int x2, int y2, float z2, kmVec3 col2,  int x3, int y3, float z3,  kmVec3 col3);
#endif

