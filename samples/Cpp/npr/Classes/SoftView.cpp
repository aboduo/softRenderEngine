#include "SoftView.h"


CCScene *SoftView::scene() {
    CCScene *sc = CCScene::create();
    SoftView *sv = SoftView::create();
    sc->addChild(sv);
    return sc;
}

bool SoftView::init(){
    if ( !CCLayer::init() )
    {
        return false;
    }
    stop = true;

    renderYet = false;

    m = new Mesh();
    //m->loadFile("testPlane.babylon");
    
    //m->loadFile("smooth.babylon");
    m->loadFile("renderCube.babylon");

    kmVec3Fill(&m->position, 0, 0, 0);
    kmQuaternionIdentity(&m->rotation); 


    plane = new Mesh();
    plane->loadFile("plane.babylon");
    kmVec3Fill(&plane->position, 0, 0, 0);
    kmQuaternionIdentity(&plane->rotation);

    /*
    kmQuaternion rx;
    kmVec3 axis;
    kmVec3Fill(&axis, 1, 0, 0);
    kmQuaternionRotationAxis(&m->rotation, &axis, 3.14/2);
    */


    /*
    m->vertices.push_back({-1, 1, 1});
    m->vertices.push_back({1, 1, 1});
    m->vertices.push_back({-1, -1, 1});
    m->vertices.push_back({-1, -1, -1});
    m->vertices.push_back({-1, 1, -1});
    m->vertices.push_back({1, 1, -1});
    m->vertices.push_back({1, -1, 1});
    m->vertices.push_back({1, -1, -1});

    m->edges.push_back({0, 1});
    m->edges.push_back({0, 2});
    m->edges.push_back({1, 6});
    m->edges.push_back({2, 6});

    m->edges.push_back({1, 5});
    m->edges.push_back({0, 4});
    m->edges.push_back({2, 3});
    m->edges.push_back({6, 7});

    m->edges.push_back({4, 5});
    m->edges.push_back({5, 7});
    m->edges.push_back({3, 7});
    m->edges.push_back({3, 4});

    m->triangles.push_back({2, 1, 0});
    m->triangles.push_back({2, 6, 1});

    m->triangles.push_back({0, 1, 4});
    m->triangles.push_back({1, 5, 4});

    m->triangles.push_back({6, 7, 5});
    m->triangles.push_back({6, 5, 1});

    m->triangles.push_back({2, 3, 6});
    m->triangles.push_back({6, 3, 7});

    m->triangles.push_back({2, 4, 3});
    m->triangles.push_back({2, 0, 4});
    
    m->triangles.push_back({3, 5, 7});
    m->triangles.push_back({3, 4, 5});
    */

    

    width = 100;
    height = 100;

    cam = new Camera(width, height);
    //左下角作为 0 0 点
    kmVec3Fill(&cam->position, 0, 2, 2);


    sp = CCSprite::create();
    addChild(sp);

    sp1 = CCSprite::create();
    addChild(sp1);


    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    sp->setPosition(ccp(200, fs.height/2));

    sp1->setPosition(ccp(600, fs.height/2));


    CCTexture2D *tex = new CCTexture2D();
    data = (unsigned char*)malloc(width*height*4);
    memset(data, 255, (width*height*4));

    CCTexture2D *tex1 = new CCTexture2D();
    data1 = (unsigned char*)malloc(width*height*4);
    memset(data1, 255, (width*height*4));

    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, width, height, CCSizeMake(width, height));
    tex1->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, width, height, CCSizeMake(width, height));
    
    //防止同一个Texture 被多次release掉
    //更新texture Data
    sp->initWithTexture(tex);

    sp->setScale(4);
    
    //sp->setScaleY(4);
    sp->setFlipY(true);
    //sp->setFlipX(true);

    sp1->setScale(4);
    sp1->initWithTexture(tex1);
    
    sp1->setFlipY(true);
    //sp1->setFlipX(true);



    //update 的时候替换 sprite的纹理
    scheduleUpdate();
    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);

    return true;
}
bool SoftView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent){
    stop = !stop;
    return true;
}


//基于照相机的 render 绘制方法
//实现的 Camera renderTarget 多屏幕绘制方法


//传递画布 tex  和 对象
void SoftView::update(float v){
    //cam->render(sp, m, data, v); 
    
    //cam->renderLine(sp, m, data, v); 
    
    if(!stop) {
        //cam->renderLine(sp1, m, data1, v); 
        //cam->renderFace(sp, m, data, v);
        //cam->renderFaceWithLight(sp, m, data, v);
        //cam->renderFaceWithTexture(sp, m, data, v);


        /*
        cam->renderLine(sp1, plane, data1, v); 
        //cam->renderFaceWithTexture(sp, plane, data, v);
        //cam->renderFaceWithLight(sp, plane, data, v);


        cam->initLightDepth();
        //make depth buffer in texture
        cam->renderLightDepth(sp, plane);
        */
        //cam->renderShadow(sp, plane, data);


        /*
        m->update(v);

        cam->initLightDepth();
        //make depth buffer in texture
        cam->renderLightDepth(sp, plane);
        cam->renderLightDepth(sp, m);
        
        //cam->showLightDepth(sp);
        //显示模型 加上 阴影 调整亮度
        cam->initBuffer(data);
        cam->renderShadow(sp, plane, data);
        //渲染立方体
        cam->renderShadow(sp, m, data);
        cam->swapBuffer(sp, data);
        */

        m->update(v);

        cam->initBuffer(data);
        cam->renderFaceTextureNPR(sp, m, data, v);
        cam->swapBuffer(sp, data);

        cam->renderLine(sp1, m, data1, v); 
    }

    if(!renderYet) {
        renderYet = true;

        /*
        cam->initBuffer(data);
        cam->renderFaceTextureNPR(sp, m, data, v);
        cam->swapBuffer(sp, data);

        cam->renderLine(sp1, m, data1, v); 
        */


        /*
        cam->initBuffer(data);
        cam->renderFaceTextureNPR(sp, m, data, v);
        cam->swapBuffer(sp, data);

        //cam->renderLine(sp1, plane, data1, v); 
        cam->renderLine(sp1, m, data1, v); 
        */
        //cam->renderFaceWithTexture(sp, plane, data, v);
        //cam->renderFaceWithLight(sp, plane, data, v);



        //cam->render(sp, m, data, v); 
    }
}

