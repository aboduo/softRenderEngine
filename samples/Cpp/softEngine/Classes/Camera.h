#ifndef __CAMERA__
#define __CAMERA__
#include "kazmath/vec3.h"
#include "cocos2d.h"
#include "Mesh.h"

using namespace cocos2d;
class Camera {
public:
    kmVec3 position;
    kmVec3 target;

    //draw Point
    void render(CCSprite *, Mesh *m, unsigned char *data, float diff);

    void renderLine(CCSprite *, Mesh *m, unsigned char *data, float diff);

    void renderFace(CCSprite *, Mesh *m, unsigned char *data, float diff);

    void update(float diff);
};

#endif
