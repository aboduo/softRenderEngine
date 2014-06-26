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

    void renderFaceWithLight(CCSprite *, Mesh *m, unsigned char *data, float diff);

    void renderFaceWithTexture(CCSprite *, Mesh *m, unsigned char *data, float diff);

    void update(float diff);

    Camera(int w, int h);


    ~Camera() {
        free(depth);
    }
private:
    CCImage *image;
    unsigned char *texture;
    int imgWidth;
    int imgHeight;

    //深度缓冲区
    float *depth;
    int width;
    int height;


};

#endif
