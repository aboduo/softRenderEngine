#ifndef __SOFTVIEW__
#define __SOFTVIEW__

#include "cocos2d.h"
#include "Mesh.h"
#include "Camera.h"
using namespace cocos2d;
class SoftView : public CCLayer {
public:
    static CCScene *scene();
    CREATE_FUNC(SoftView);
    bool init();

    virtual void update(float);


    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    /*
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    */
private:
    bool stop;
    CCSprite *sp, *sp1;
    CCTexture2D *tex;
    unsigned char *data, *data1;

    int width;
    int height;


    Mesh *m;
    Camera *cam;

    bool renderYet;
};
#endif
