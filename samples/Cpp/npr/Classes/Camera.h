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

    void renderFaceTextureNPR(CCSprite *, Mesh *m, unsigned char *data, float diff);


    //从光源渲染对象 到深度缓冲区
    //初始化一下光照缓冲区
    void initLightDepth();
    void renderLightDepth(CCSprite *sp, Mesh *m);

    void initBuffer(unsigned char *data);
    void renderShadow(CCSprite *sp, Mesh *m, unsigned char *data);

    void swapBuffer(CCSprite *sp, unsigned char *data);


    void showLightDepth(CCSprite *sp);



    void update(float diff);

    Camera(int w, int h);


    ~Camera() {
        free(depth);
        free(lightDepthBuffer);
        free(lightBuffer);
    }
private:
    //从光源方向的深度缓冲区
    float *lightDepthBuffer;

    unsigned char *lightBuffer;
    

    //顶点有个问题 没有办法得到每个像素点的最终深度值 无法进行比较了
    //像素重叠
    //vector<kmVec3> lightDepth;


    CCImage *image;
    unsigned char *texture;
    
    unsigned char *nprShade;
    CCSize nprSize;


    int imgWidth;
    int imgHeight;

    //深度缓冲区
    float *depth;
    int width;
    int height;


};

#endif
