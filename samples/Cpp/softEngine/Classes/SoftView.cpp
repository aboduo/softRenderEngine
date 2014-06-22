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

    m = new Mesh();
    m->vertices.push_back({-1, 1, 1});
    m->vertices.push_back({1, 1, 1});
    m->vertices.push_back({-1, -1, 1});
    m->vertices.push_back({-1, -1, -1});
    m->vertices.push_back({-1, 1, -1});
    m->vertices.push_back({1, 1, -1});
    m->vertices.push_back({1, -1, 1});
    m->vertices.push_back({1, -1, -1});
    kmVec3Fill(&m->position, 0, 0, 0);
    kmQuaternionIdentity(&m->rotation); 

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


    width = 100;
    height = 100;

    cam = new Camera();
    //左下角作为 0 0 点
    kmVec3Fill(&cam->position, 0, 0, 5);


    sp = CCSprite::create();
    addChild(sp);

    CCSize fs = CCDirector::sharedDirector()->getVisibleSize();
    sp->setPosition(CCSizeMake(fs.width/2, fs.height/2));


    tex = new CCTexture2D();
    data = (unsigned char*)malloc(width*height*4);
    memset(data, 255, (width*height*4));

    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, width, height, CCSizeMake(width, height));
    
    //更新texture Data
    sp->initWithTexture(tex);

    sp->setScale(4);
    //sp->setFlipY(true);
    sp->setFlipX(true);


    //update 的时候替换 sprite的纹理
    scheduleUpdate();

    return true;
}


//基于照相机的 render 绘制方法
//实现的 Camera renderTarget 多屏幕绘制方法


//传递画布 tex  和 对象
void SoftView::update(float v){
    //cam->render(sp, m, data, v); 
    //cam->renderLine(sp, m, data, v); 
    cam->renderFace(sp, m, data, v); 
}

