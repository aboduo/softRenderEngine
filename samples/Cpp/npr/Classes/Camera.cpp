#include "Camera.h"
#include "kazmath/mat4.h"
#include "kazmath/vec4.h"
#include "kazmath/quaternion.h"
#include "util.h"
#include "Light.h"
#include "NormalMap.h"


using namespace std;


static void printMat(kmMat4 *mat) {
    /*
    printf("mat \n");
    for(int i = 0; i < 16; i++) {
        printf("%f ", mat->mat[(i%4)*4+i/4]);
        if(i %4 == 3) {
            printf("\n");
        }
    }
    printf("\n");
    */
}

Camera::Camera(int w, int h):
width(w),
height(h)
{
    depth = (float*)malloc(width*height*sizeof(float));
    lightDepthBuffer = (float*)malloc(width*height*sizeof(float));
    lightBuffer = (unsigned char*)malloc(width*height*sizeof(unsigned char));
    normalMap = (float*)malloc(width*height*sizeof(float));

    memset(depth, 0, width*height*sizeof(float));
    
    image = new CCImage();
    //bool loadSuc = image->initWithImageFile("testPlane2.png");
    //bool loadSuc = image->initWithImageFile("smooth.png");

    bool loadSuc = image->initWithImageFile("cubeMap.png");
    //CCLog("load image suc %d", loadSuc);

    texture = image->getData();
    imgWidth = image->getWidth();
    imgHeight = image->getHeight();


    CCImage *im2 = new CCImage();
    //im2->initWithImageFile("64shade.png");
    im2->initWithImageFile("shade.png");
    nprShade = im2->getData();
    nprSize = CCSizeMake(im2->getWidth(), im2->getHeight());

    //CCLog("image size %d %d", imgWidth, imgHeight);
}


void Camera::update(float diff) {
}


void Camera::renderFace(CCSprite *sp, Mesh *m, unsigned char *data, float diff) {
    //m->update(diff);

    CCSize tsz = sp->getContentSize(); 
    //投影每个mesh的顶点 计算2d 空间坐标
    vector<kmVec4> npos;

    //ModelView Matrix
    //Project Matrix 两个矩阵进行计算

    kmMat4 matrixPers;
    kmMat4 matrixLookup;
    kmMat4 matrixModel;

    kmMat4PerspectiveProjection(&matrixPers, 60, tsz.width/tsz.height, 0.2f, 30);
    /*
    printf("mat Look\n");
    for(int i = 0; i < 16; i++) {
        printf("%f ", matrixPers.mat[(i%4)*4+i/4]);
        if(i %4 == 3) {
            printf("\n");
        }
    }
    printf("\n");
    */


    int cx = tsz.width/2;
    int cy = tsz.height/2;

    kmVec3 eye, center, up;
    kmVec3Fill( &eye, position.x, position.y,  position.z);
    kmVec3Fill( &center, 0, 0, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);
    
    kmMat3 rot;
    kmMat3RotationQuaternion(&rot, &m->rotation);
    kmMat4RotationTranslation(&matrixModel, &rot, &m->position);

    printMat(&matrixPers);

    printMat(&matrixLookup);

    printMat(&matrixModel);


    kmMat4 tempMat;
    kmMat4Multiply(&tempMat, &matrixPers, &matrixLookup);

    kmMat4 allMat;
    kmMat4Multiply(&allMat, &tempMat, &matrixModel);

    printMat(&allMat);

    //Model matrix
    for(int i=0;i < m->vertices.size(); i++) {
        kmVec4 out;
        kmVec4 temp;
        kmVec3 vt = m->vertices[i];
        kmVec4Fill(&temp, vt.x, vt.y, vt.z, 1);

        //kmVec4Transform(&out, &temp, &matrixLookup);



        kmVec4 out2;
        //kmVec4Transform(&out2, &out, &matrixPers);
        kmVec4Transform(&out2, &temp, &allMat);

        npos.push_back(out2);
    }
    


    memset(data, 0, (tsz.width*tsz.height*4));
    memset(depth, 0, width*height*sizeof(float));

    //clear buffer
    //CCLog("start %f %f", tsz.width, tsz.height);
    //数据的Y 方向似乎不对
    //图像数据 是 左上角 0 0 坐标的 因此 Y 方向就不对了

    //绘制 line 最简单的 绘线算法
    //旋转X 方向 反了 为什么呢?
    /*
    for(int i=0; i < m->edges.size(); i++) {
        int a = m->edges[i].a;
        int b = m->edges[i].b;
        
        float apx = npos[a].x;
        float apy = npos[a].y;
        float aw = npos[a].w;

        float bpx = npos[b].x;
        float bpy = npos[b].y;
        float bw = npos[b].w;

        //CCLog("axy bxy %f %f %f %f %f %f", apx, apy, aw, bpx, bpy, bw);
        //根据角度绘制x y 方向角度绘制线
        drawLine(data, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, apy/aw*tsz.height/2+cy, bpx/bw*tsz.width/2+cx, bpy/bw*tsz.height/2+cy, 1);
    }
    */
    
    for(int i=0; i < m->triangles.size(); i++) {
        int a = m->triangles[i].a;
        int b = m->triangles[i].b;
        int c = m->triangles[i].c;
        
        //CCLog("aa %d %d %d", a, b, c);
        float apx = npos[a].x;
        float apy = npos[a].y;
        float apz = npos[a].z;
        float aw = npos[a].w;

        float bpx = npos[b].x;
        float bpy = npos[b].y;
        float bpz = npos[b].z;
        float bw = npos[b].w;
        
        float cpx = npos[c].x;
        float cpy = npos[c].y;
        float cpz = npos[c].z;
        float cw = npos[c].w;
    
        //CCLog("a b c %f %f %f  %f %f %f  %f %f %f", apx, apy, aw, bpx, bpy, bw, cpx, cpy, cw);
        //根据 zbuffer数据 更新depth数据
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, tsz.height-(apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, tsz.height-(bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, tsz.height-(cpy/cw*tsz.height/2+cy), cpz/cw, i);
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
        drawFace(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
    }


    

    //CCLog("end");

    CCTexture2D *oldTex = sp->getTexture();
    oldTex->release();

    CCTexture2D *tex = new CCTexture2D();
    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, tsz.width, tsz.height, CCSizeMake(tsz.width, tsz.height));
    //sp->setFlipY(true);
    sp->setTexture(tex);

    //调整纹理 Y 方向 纹理坐标参数
    //sp->setFlipY(false);
    //float sca = sp->getScaleX();
    //sp->setScaleY(-sca);
}

void Camera::renderLine(CCSprite *sp, Mesh *m, unsigned char *data, float diff) {
    //m->update(diff);

    CCSize tsz = sp->getContentSize(); 
    //投影每个mesh的顶点 计算2d 空间坐标
    vector<kmVec4> npos;

    //ModelView Matrix
    //Project Matrix 两个矩阵进行计算

    kmMat4 matrixPers;
    kmMat4 matrixLookup;
    kmMat4 matrixModel;

    kmMat4PerspectiveProjection(&matrixPers, 60, tsz.width/tsz.height, 0.2f, 30);
    /*
    printf("mat Look\n");
    for(int i = 0; i < 16; i++) {
        printf("%f ", matrixPers.mat[(i%4)*4+i/4]);
        if(i %4 == 3) {
            printf("\n");
        }
    }
    printf("\n");
    */


    int cx = tsz.width/2;
    int cy = tsz.height/2;

    kmVec3 eye, center, up;
    kmVec3Fill( &eye, position.x, position.y,  position.z);
    kmVec3Fill( &center, 0, 0, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);
    
    kmMat3 rot;
    kmMat3RotationQuaternion(&rot, &m->rotation);
    kmMat4RotationTranslation(&matrixModel, &rot, &m->position);

    printMat(&matrixPers);

    printMat(&matrixLookup);

    printMat(&matrixModel);


    kmMat4 tempMat;
    kmMat4Multiply(&tempMat, &matrixPers, &matrixLookup);

    kmMat4 allMat;
    kmMat4Multiply(&allMat, &tempMat, &matrixModel);

    printMat(&allMat);

    //Model matrix
    for(int i=0;i < m->vertices.size(); i++) {
        kmVec4 out;
        kmVec4 temp;
        kmVec3 vt = m->vertices[i];
        kmVec4Fill(&temp, vt.x, vt.y, vt.z, 1);

        //kmVec4Transform(&out, &temp, &matrixLookup);



        kmVec4 out2;
        //kmVec4Transform(&out2, &out, &matrixPers);
        kmVec4Transform(&out2, &temp, &allMat);

        npos.push_back(out2);
    }
    


    memset(data, 0, (tsz.width*tsz.height*4));
    //clear buffer
    //CCLog("start %f %f", tsz.width, tsz.height);
    //数据的Y 方向似乎不对
    //图像数据 是 左上角 0 0 坐标的 因此 Y 方向就不对了

    //绘制 line 最简单的 绘线算法
    //旋转X 方向 反了 为什么呢?
    for(int i=0; i < m->edges.size(); i++) {
        int a = m->edges[i].a;
        int b = m->edges[i].b;
        
        float apx = npos[a].x;
        float apy = npos[a].y;
        float aw = npos[a].w;

        float bpx = npos[b].x;
        float bpy = npos[b].y;
        float bw = npos[b].w;

        //CCLog("axy bxy %f %f %f %f %f %f", apx, apy, aw, bpx, bpy, bw);
        //根据角度绘制x y 方向角度绘制线
        drawLine(data, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, apy/aw*tsz.height/2+cy, bpx/bw*tsz.width/2+cx, bpy/bw*tsz.height/2+cy, i);
    }



    //CCLog("end");

    CCTexture2D *oldTex = sp->getTexture();
    oldTex->release();

    CCTexture2D *tex = new CCTexture2D();
    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, tsz.width, tsz.height, CCSizeMake(tsz.width, tsz.height));
    //sp->setFlipY(true);
    sp->setTexture(tex);

    //调整纹理 Y 方向 纹理坐标参数
    //sp->setFlipY(false);
    //float sca = sp->getScaleX();
    //sp->setScaleY(-sca);
}



void Camera::render(CCSprite *sp, Mesh *m, unsigned char *data, float diff) {
    //m->update(diff);

    CCSize tsz = sp->getContentSize(); 
    //投影每个mesh的顶点 计算2d 空间坐标
    vector<kmVec4> npos;

    //ModelView Matrix
    //Project Matrix 两个矩阵进行计算

    kmMat4 matrixPers;
    kmMat4 matrixLookup;
    kmMat4 matrixModel;

    kmMat4PerspectiveProjection(&matrixPers, 60, tsz.width/tsz.height, 0.2f, 30);
    /*
    printf("mat Look\n");
    for(int i = 0; i < 16; i++) {
        printf("%f ", matrixPers.mat[(i%4)*4+i/4]);
        if(i %4 == 3) {
            printf("\n");
        }
    }
    printf("\n");
    */



    kmVec3 eye, center, up;
    kmVec3Fill( &eye, position.x, position.y,  position.z);
    kmVec3Fill( &center, 0, 0, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);
    
    kmMat3 rot;
    kmMat3RotationQuaternion(&rot, &m->rotation);
    kmMat4RotationTranslation(&matrixModel, &rot, &m->position);

    printMat(&matrixPers);

    printMat(&matrixLookup);

    printMat(&matrixModel);


    kmMat4 tempMat;
    kmMat4Multiply(&tempMat, &matrixPers, &matrixLookup);

    kmMat4 allMat;
    kmMat4Multiply(&allMat, &tempMat, &matrixModel);

    printMat(&allMat);

    //Model matrix
    for(int i=0;i < m->vertices.size(); i++) {
    //for(int i=0;i < 1; i++) {
        kmVec4 out;
        kmVec4 temp;
        kmVec3 vt = m->vertices[i];
        kmVec4Fill(&temp, vt.x, vt.y, vt.z, 1);

        //kmVec4Transform(&out, &temp, &matrixLookup);



        kmVec4 out2;
        //kmVec4Transform(&out2, &out, &matrixPers);
        kmVec4Transform(&out2, &temp, &allMat);

        npos.push_back(out2);
    }
    
    int cx = tsz.width/2;
    int cy = tsz.height/2;


    memset(data, 0, (tsz.width*tsz.height*4));
    //clear buffer
    //CCLog("start %f %f", tsz.width, tsz.height);
    //数据的Y 方向似乎不对
    //图像数据 是 左上角 0 0 坐标的 因此 Y 方向就不对了

    //旋转X 方向 反了 为什么呢?
    for(int i=0; i < npos.size(); i++) {
        //CCLog("over pos %f %f %f %f", npos[i].x, npos[i].y, npos[i].z, npos[i].w);
        //CCLog("depth %f", npos[i].z/npos[i].w);

        float px = npos[i].x;
        float py = npos[i].y;
        float w = npos[i].w;

        //缩放一下尺寸
        int vx = std::min(std::max(0.0f, (px/w*tsz.width/2+cx)), tsz.width-1.0f);
        //Y 方向反转一下
        int vy = std::min(std::max(0.0f, (py/w*tsz.height/2+cy)), tsz.height-1.0f);
        
        //CCLog("vx vy %d %d", vx, vy);
        
        data[int(vy*tsz.width+vx)*4+0] = 255; 
        data[int(vy*tsz.width+vx)*4+1] = 255; 
        data[int(vy*tsz.width+vx)*4+2] = 255; 
        data[int(vy*tsz.width+vx)*4+3] = 255; 
    }

    //CCLog("end");

    CCTexture2D *oldTex = sp->getTexture();
    oldTex->release();

    CCTexture2D *tex = new CCTexture2D();
    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, tsz.width, tsz.height, CCSizeMake(tsz.width, tsz.height));
    //sp->setFlipY(true);
    sp->setTexture(tex);

    //调整纹理 Y 方向 纹理坐标参数
    //sp->setFlipY(false);
    //float sca = sp->getScaleX();
    //sp->setScaleY(-sca);

}


//bug if depth 0 appear in a place depth error will appear 
void Camera::initLightDepth() {
    memset(lightDepthBuffer, 0, (width*height*sizeof(float)));
}


//类似于将镜头移动到 光源位置然后 渲染一遍场景深度
//计算每个定点的 光源深度 定点构成的平面的深度只是 
void Camera::renderLightDepth(CCSprite *sp, Mesh *m) {
    CCSize tsz = sp->getContentSize(); 
    //投影每个mesh的顶点 计算2d 空间坐标
    vector<kmVec4> npos;
    //vector<kmVec3> nnormal;

    int cx = tsz.width/2;
    int cy = tsz.height/2;
    //each vertex color
    //vector<kmVec3> color;
    

    kmMat4 matrixPers;
    kmMat4 matrixLookup;
    kmMat4 matrixModel;

    kmMat4PerspectiveProjection(&matrixPers, 60, tsz.width/tsz.height, 0.2f, 30);


    //镜头位置 以及镜头方向 
    kmVec3 eye, center, up;
    kmVec3Fill( &eye, 2, 2,  2);
    kmVec3Fill( &center, 0, 0, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);
    
    kmMat3 rot;
    kmMat3RotationQuaternion(&rot, &m->rotation);
    kmMat4RotationTranslation(&matrixModel, &rot, &m->position);


    kmMat4 tempMat;
    kmMat4Multiply(&tempMat,  &matrixLookup, &matrixModel);

    kmMat4 allMat;
    kmMat4Multiply(&allMat, &matrixPers, &tempMat);
    printMat(&allMat);

    for(int i=0;i < m->vertices.size(); i++) {
        kmVec4 out;
        kmVec4 temp;
        kmVec3 vt = m->vertices[i];
        kmVec4Fill(&temp, vt.x, vt.y, vt.z, 1);

        kmVec4 out2;
        kmVec4Transform(&out2, &temp, &allMat);

        npos.push_back(out2);
    }

    for(int i=0; i < m->triangles.size(); i++) {
        int a = m->triangles[i].a;
        int b = m->triangles[i].b;
        int c = m->triangles[i].c;
        
        //CCLog("aa %d %d %d", a, b, c);
        float apx = npos[a].x;
        float apy = npos[a].y;
        float apz = npos[a].z;
        float aw = npos[a].w;

        float bpx = npos[b].x;
        float bpy = npos[b].y;
        float bpz = npos[b].z;
        float bw = npos[b].w;
        
        float cpx = npos[c].x;
        float cpy = npos[c].y;
        float cpz = npos[c].z;
        float cw = npos[c].w;
    
        //CCLog("a b c %f %f %f  %f %f %f  %f %f %f", apx, apy, aw, bpx, bpy, bw, cpx, cpy, cw);
        //根据 zbuffer数据 更新depth数据
        drawLightDepth(lightDepthBuffer, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw);
    }

    //CCLog("height matrix");

}


void Camera::showLightDepth(CCSprite *sp) {
    for(int i = 0; i < height; i++) {
        for(int j=0; j < width; j++) {
            float depth = lightDepthBuffer[i*width+j];
            printf("%.2f ", depth);
            lightBuffer[i*width+j] = depth*255;
        }
        printf("\n");
    }


    //render ligth depth out
    //avoid release for multiple times
    CCTexture2D *oldTex = sp->getTexture();
    //if(oldTex)
    oldTex->release();
    //CCLog("remove old Texture");

    CCTexture2D *tex = new CCTexture2D();
    //tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, tsz.width, tsz.height, CCSizeMake(tsz.width, tsz.height));

    tex->initWithData(lightBuffer, kCCTexture2DPixelFormat_I8, width, height, CCSizeMake(width, height));

    //GL_FLOAT 存在问题传入的数值
    //tex->initWithData(lightDepthBuffer, kCCTexture2DPixelFormat_I8Float, tsz.width, tsz.height, CCSizeMake(tsz.width, tsz.height));

    //sp->setFlipY(true);
    sp->setTexture(tex);
    
    //CCLog("set new Texture");
}

//绘制Mesh  计算屏幕空间坐标  计算light空间坐标插值
//从 lightDepthBuffer 中 获取 light的深度值
//对于超出lightDepthBuffer 的部分不用理会 坐标问题 加上雾气即可

void Camera::renderShadow(CCSprite *sp, Mesh *m, unsigned char *data) {
    //CCSize tsz = sp->getContentSize(); 
    //投影每个mesh的顶点 计算2d 空间坐标
    vector<kmVec4> npos;
    //vector<kmVec4> nposLight;

    //vector<kmVec3> nnormal;

    //each vertex color
    //vector<kmVec3> color;
    kmVec3 light = {2, 2, 2};

    kmMat4 matrixPers;
    kmMat4 matrixPersLight;

    kmMat4 matrixLookup;
    kmMat4 matrixLookupLight;

    kmMat4 matrixModel;
    kmMat4 matrixModelLight;

    kmMat4PerspectiveProjection(&matrixPers, 60, width/height, 0.2f, 30);
    kmMat4PerspectiveProjection(&matrixPersLight, 60, width/height, 0.2f, 30);

    int cx = width/2;
    int cy = height/2;

    kmVec3 eye, center, up;
    kmVec3Fill( &eye, position.x, position.y,  position.z);
    kmVec3Fill( &center, 0, 0, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);

    kmVec3Fill( &eye, light.x, light.y,  light.z);
    kmVec3Fill( &center, 0, 0, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookupLight, &eye, &center, &up);

    
    kmMat3 rot;
    kmMat3RotationQuaternion(&rot, &m->rotation);
    kmMat4RotationTranslation(&matrixModel, &rot, &m->position);


    //kmMat3 rot;
    //kmMat3RotationQuaternion(&rot, &m->rotation);
    kmMat4RotationTranslation(&matrixModelLight, &rot, &m->position);


    kmMat4 tempMat, tempMatLight;
    kmMat4Multiply(&tempMat,  &matrixLookup, &matrixModel);
    kmMat4Multiply(&tempMatLight,  &matrixLookupLight, &matrixModelLight);

    kmMat4 allMat, allMatLight;
    kmMat4Multiply(&allMat, &matrixPers, &tempMat);
    kmMat4Multiply(&allMatLight, &matrixPersLight, &tempMatLight);

    printMat(&allMat);
    printMat(&allMatLight);
    
    //CCLog("two matrix");
    printMat(&allMat);
    //CCLog("light matrix");
    printMat(&allMatLight);

    for(int i=0;i < m->vertices.size(); i++) {
        kmVec4 out;
        kmVec4 temp;
        kmVec3 vt = m->vertices[i];
        //world position
        kmVec4Fill(&temp, vt.x, vt.y, vt.z, 1);

        kmVec4 out2, outLight;
        
        kmVec4Transform(&out2, &temp, &allMat);
        
        //kmVec4Transform(&out2, &temp, &allMatLight);
        
        //kmVec4Transform(&outLight, &temp, &allMatLight);

        npos.push_back(out2);
        //nposLight.push_back(outLight);
    }


    for(int i=0; i < m->triangles.size(); i++) {
        int a = m->triangles[i].a;
        int b = m->triangles[i].b;
        int c = m->triangles[i].c;
        
        //CCLog("aa %d %d %d", a, b, c);
        float apx = npos[a].x;
        float apy = npos[a].y;
        float apz = npos[a].z;
        float aw = npos[a].w;

    /*
        float lapx = nposLight[a].x;
        float lapy = nposLight[a].y;
        float lapz = nposLight[a].z;
        float law = nposLight[a].w;
    */

        float bpx = npos[b].x;
        float bpy = npos[b].y;
        float bpz = npos[b].z;
        float bw = npos[b].w;
    /*
        float lbpx = nposLight[b].x;
        float lbpy = nposLight[b].y;
        float lbpz = nposLight[b].z;
        float lbw = nposLight[b].w;
    */

        float cpx = npos[c].x;
        float cpy = npos[c].y;
        float cpz = npos[c].z;
        float cw = npos[c].w;
    
    /*
        float lcpx = nposLight[c].x;
        float lcpy = nposLight[c].y;
        float lcpz = nposLight[c].z;
        float lcw = nposLight[c].w;
    */

        //CCLog("a b c %f %f %f  %f %f %f  %f %f %f", apx, apy, aw, bpx, bpy, bw, cpx, cpy, cw);
        //根据 zbuffer数据 更新depth数据
        //CCLog("depth %f %f %f", apz/aw, bpz/bw, cpz/cw);
        
        kmVec3 p1 = {apx/aw*cx+cx, (apy/aw*cy+cy), apz/aw};
        kmVec3 p2 = {bpx/bw*cx+cx, (bpy/bw*cy+cy), bpz/bw};
        kmVec3 p3 = {cpx/cw*cx+cx, (cpy/cw*cy+cy), cpz/cw};
        
        //从光源角度 看的顶点坐标 或者 在pixel shader 中重新反向计算一下? 
        //传入matrix 矩阵?
        //pixel coordinate
        /*
        kmVec3 lp1 = {lapx/law*width/2+cx, (lapy/law*height/2+cy), lapz/law};
        kmVec3 lp2 = {lbpx/lbw*width/2+cx, (lbpy/lbw*height/2+cy), lbpz/lbw};
        kmVec3 lp3 = {lcpx/lcw*width/2+cx, (lcpy/lcw*height/2+cy), lcpz/lcw};
        */
    
        //light transform matrix 
        //worldPosition of pixel
        drawShadow(data, depth, lightDepthBuffer, width, height, allMatLight, p1, m->vertices[a],  p2, m->vertices[b],  p3, m->vertices[c]);

        //drawFace(data, depth, width, height, apx/aw*width/2+cx, (apy/aw*height/2+cy), apz/aw, bpx/bw*width/2+cx, (bpy/bw*height/2+cy), bpz/bw, cpx/cw*width/2+cx, (cpy/cw*height/2+cy), cpz/cw, i);
    }

    //CCLog("depth data");
    
}

void Camera::swapBuffer(CCSprite *sp, unsigned char *data) {
    CCTexture2D *oldTex = sp->getTexture();
    oldTex->release();
    //CCLog("remove old Texture");

    CCTexture2D *tex = new CCTexture2D();
    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, width, height, CCSizeMake(width, height));
    sp->setTexture(tex);
}

void Camera::initBuffer(unsigned char *data) {
    memset(data, 0, (width*height*4));
    //memset(depth, 0, width*height*sizeof(float));
    for(int i = 0; i < width*height; i++) {
        depth[i] = 1;
    }
}


void Camera::renderFaceWithLight(CCSprite *sp, Mesh *m, unsigned char *data, float diff) {
    //m->update(diff);

    CCSize tsz = sp->getContentSize(); 
    //投影每个mesh的顶点 计算2d 空间坐标
    vector<kmVec4> npos;
    vector<kmVec3> nnormal;

    //each vertex color
    vector<kmVec3> color;

    //ModelView Matrix
    //Project Matrix 两个矩阵进行计算

    kmMat4 matrixPers;
    kmMat4 matrixLookup;
    kmMat4 matrixModel;

    kmMat4PerspectiveProjection(&matrixPers, 60, tsz.width/tsz.height, 0.2f, 30);


    int cx = tsz.width/2;
    int cy = tsz.height/2;

    kmVec3 eye, center, up;
    kmVec3Fill( &eye, position.x, position.y,  position.z);
    kmVec3Fill( &center, 0, 0, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);
    
    kmMat3 rot;
    kmMat3RotationQuaternion(&rot, &m->rotation);
    kmMat4RotationTranslation(&matrixModel, &rot, &m->position);

    printMat(&matrixPers);

    printMat(&matrixLookup);

    printMat(&matrixModel);


    kmMat4 tempMat;
    kmMat4Multiply(&tempMat,  &matrixLookup, &matrixModel);

    kmMat4 allMat;
    kmMat4Multiply(&allMat, &matrixPers, &tempMat);
    printMat(&allMat);


    //only rotation  no translation
    //世界坐标中 提取出旋转 矩阵应用到 normal 上面
    //kmMat3 normalMatrix;
    //kmMat4ExtractRotation(&normalMatrix, &matrixModel);
    //printMat(&normalMatrix);

    kmVec3 light = {2, 2, 2};

    //Model matrix
    for(int i=0;i < m->vertices.size(); i++) {
        kmVec4 out;
        kmVec4 temp;
        kmVec3 vt = m->vertices[i];
        kmVec4Fill(&temp, vt.x, vt.y, vt.z, 1);

        //kmVec4Transform(&out, &temp, &matrixLookup);



        kmVec4 out2;
        //kmVec4Transform(&out2, &out, &matrixPers);
        kmVec4Transform(&out2, &temp, &allMat);

        npos.push_back(out2);
    
        //世界坐标重新计算normal light 位置不变
        kmVec3 nn;
        kmVec3TransformNormal(&nn, &m->normal[i], &matrixModel);
        nnormal.push_back(nn);


        kmVec3 worldPosition;
        kmVec3Transform(&worldPosition, &vt, &matrixModel);
        
        //diff = 
        kmVec3 tempRes;
        kmVec3 lightDir;
        kmVec3Subtract(&tempRes, &light, &worldPosition);
        kmVec3Normalize(&lightDir, &tempRes);
        
        //cosin  光线方向 和 normal 方向
        kmVec3Normalize(&nn, &nn);
        float cosTheta = kmVec3Dot(&lightDir, &nn);
        cosTheta = std::max(0.0f, cosTheta);

        //CCLog("color is %f", cosTheta);
        color.push_back({cosTheta, cosTheta, cosTheta});
    }
    


    memset(data, 0, (tsz.width*tsz.height*4));
    memset(depth, 0, width*height*sizeof(float));

    //clear buffer
    //CCLog("start %f %f", tsz.width, tsz.height);
    //数据的Y 方向似乎不对
    //图像数据 是 左上角 0 0 坐标的 因此 Y 方向就不对了

    //绘制 line 最简单的 绘线算法
    //旋转X 方向 反了 为什么呢?
    /*
    for(int i=0; i < m->edges.size(); i++) {
        int a = m->edges[i].a;
        int b = m->edges[i].b;
        
        float apx = npos[a].x;
        float apy = npos[a].y;
        float aw = npos[a].w;

        float bpx = npos[b].x;
        float bpy = npos[b].y;
        float bw = npos[b].w;

        //CCLog("axy bxy %f %f %f %f %f %f", apx, apy, aw, bpx, bpy, bw);
        //根据角度绘制x y 方向角度绘制线
        drawLine(data, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, apy/aw*tsz.height/2+cy, bpx/bw*tsz.width/2+cx, bpy/bw*tsz.height/2+cy, 1);
    }
    */
    
    for(int i=0; i < m->triangles.size(); i++) {
        int a = m->triangles[i].a;
        int b = m->triangles[i].b;
        int c = m->triangles[i].c;
        
        //CCLog("aa %d %d %d", a, b, c);
        float apx = npos[a].x;
        float apy = npos[a].y;
        float apz = npos[a].z;
        float aw = npos[a].w;

        float bpx = npos[b].x;
        float bpy = npos[b].y;
        float bpz = npos[b].z;
        float bw = npos[b].w;
        
        float cpx = npos[c].x;
        float cpy = npos[c].y;
        float cpz = npos[c].z;
        float cw = npos[c].w;
    
        //CCLog("a b c %f %f %f  %f %f %f  %f %f %f", apx, apy, aw, bpx, bpy, bw, cpx, cpy, cw);
        //根据 zbuffer数据 更新depth数据
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, tsz.height-(apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, tsz.height-(bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, tsz.height-(cpy/cw*tsz.height/2+cy), cpz/cw, i);
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
        //drawFace(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
        
        drawFaceWithLight(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, color[a], bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, color[b], cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, color[c]);

    }


    

    //CCLog("end");

    CCTexture2D *oldTex = sp->getTexture();
    //if(oldTex)
    oldTex->release();
    //CCLog("remove old Texture");

    CCTexture2D *tex = new CCTexture2D();
    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, tsz.width, tsz.height, CCSizeMake(tsz.width, tsz.height));
    //sp->setFlipY(true);
    sp->setTexture(tex);
    
    //CCLog("set new Texture");


    //调整纹理 Y 方向 纹理坐标参数
    //sp->setFlipY(false);
    //float sca = sp->getScaleX();
    //sp->setScaleY(-sca);
}


/*
采用类似光照强度的算法进行渲染
方向光照 lightDir  -1 -1 -1 
*/
void Camera::renderFaceTextureNPR(CCSprite *sp, Mesh *m, unsigned char *data, float diff) {
    vector<kmVec4> npos;
    vector<kmVec3> nnormal;
    //每个顶点的diffuse 值
    vector<float> color;

    kmMat4 matrixPers;
    kmMat4 matrixLookup;
    kmMat4 matrixModel;

    kmMat4PerspectiveProjection(&matrixPers, 60, width/height, 0.2f, 30);
    
        
    int cx = width/2;
    int cy = height/2;

    kmVec3 eye, center, up;
    kmVec3Fill( &eye, position.x, position.y,  position.z);
    kmVec3Fill( &center, 0, 0, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);
    
    kmMat3 rot;
    kmMat3RotationQuaternion(&rot, &m->rotation);
    kmMat4RotationTranslation(&matrixModel, &rot, &m->position);

    printMat(&matrixPers);

    printMat(&matrixLookup);

    printMat(&matrixModel);


    kmMat4 tempMat;
    kmMat4Multiply(&tempMat,  &matrixLookup, &matrixModel);

    kmMat4 allMat;
    kmMat4Multiply(&allMat, &matrixPers, &tempMat);
    printMat(&allMat);


    kmMat4 lightViewMat;
    kmMat4Multiply(&lightViewMat, &matrixPers, &matrixLookup);

    
    //要和normal 同方向
    kmVec3 tempDir = {0.5, 1, 0.5};
    kmVec3 lightDir;
    kmVec3Normalize(&lightDir, &tempDir);

    Light li;
    kmVec3Fill(&li.position, 0.5, 0.5, 0.5);
    kmVec3Fill(&li.direction, 0.5, 1, 0.5);
    li.calculateEdge();


    //Model matrix
    for(int i=0;i < m->vertices.size(); i++) {
        kmVec4 out;
        kmVec4 temp;
        kmVec3 vt = m->vertices[i];
        kmVec4Fill(&temp, vt.x, vt.y, vt.z, 1);

        kmVec4 out2;
        kmVec4Transform(&out2, &temp, &allMat);
        npos.push_back(out2);
    
        //世界坐标重新计算normal light 位置不变
        kmVec3 nn;
        kmVec3TransformNormal(&nn, &m->normal[i], &matrixModel);
        kmVec3Normalize(&nn, &nn);
        nnormal.push_back(nn);

        //kmVec3 worldPosition;
        //kmVec3Transform(&worldPosition, &vt, &matrixModel);
        
        //diff = 
        //kmVec3 tempRes;
        //kmVec3 lightDir;
        //kmVec3Subtract(&tempRes, &light, &worldPosition);
        //kmVec3Normalize(&lightDir, &tempRes);
        
        //cosin  光线方向 和 normal 方向
        

        //float cosTheta = kmVec3Dot(&lightDir, &nn);
        //cosTheta = std::max(0.0f, cosTheta);

        //CCLog("color is %f", cosTheta);
        //color.push_back(cosTheta);
    }

    NormalMap nm;


    // texture value  color 
    for(int i=0; i < m->triangles.size(); i++) {
        int a = m->triangles[i].a;
        int b = m->triangles[i].b;
        int c = m->triangles[i].c;
        
        nm.centerPos.push_back(npos[a]);
        nm.centerPos.push_back(npos[b]);
        nm.centerPos.push_back(npos[c]);

        nm.normalMap.push_back(nnormal[a]);
        nm.normalMap.push_back(nnormal[b]);
        nm.normalMap.push_back(nnormal[c]);

        //CCLog("aa %d %d %d", a, b, c);
        float apx = npos[a].x;
        float apy = npos[a].y;
        float apz = npos[a].z;
        float aw = npos[a].w;

        float bpx = npos[b].x;
        float bpy = npos[b].y;
        float bpz = npos[b].z;
        float bw = npos[b].w;
        
        float cpx = npos[c].x;
        float cpy = npos[c].y;
        float cpz = npos[c].z;
        float cw = npos[c].w;
    
        //CCLog("a b c %f %f %f  %f %f %f  %f %f %f", apx, apy, aw, bpx, bpy, bw, cpx, cpy, cw);
        //根据 zbuffer数据 更新depth数据
        //drawFaceWithLight(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, color[a], bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, color[b], cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, color[c]);
        
        //drawFaceWithTexture(data, depth, texture, width, height, imgWidth, imgHeight, apx/aw*cx+cx, (apy/aw*cy+cy), apz/aw, m->textureCoord[a], bpx/bw*cx+cx, (bpy/bw*cy+cy), bpz/bw, m->textureCoord[b], cpx/cw*cx+cx, (cpy/cw*cy+cy), cpz/cw, m->textureCoord[c]);

        kmVec3 p1 = {apx/aw*cx+cx, apy/aw*cy+cy, apz/aw};
        kmVec3 p2 = {bpx/bw*cx+cx, bpy/bw*cy+cy, bpz/bw};
        kmVec3 p3 = {cpx/cw*cx+cx, cpy/cw*cy+cy, cpz/cw};
        
        /*
        PosTexDiff pt1 = {p1, m->textureCoord[a], color[a] };
        PosTexDiff pt2 = {p2, m->textureCoord[b], color[b] };
        PosTexDiff pt3 = {p3, m->textureCoord[c], color[c] };
        */

        PosTexNor pt1 = {p1, m->textureCoord[a], nnormal[a] };
        PosTexNor pt2 = {p2, m->textureCoord[b], nnormal[b] };
        PosTexNor pt3 = {p3, m->textureCoord[c], nnormal[c] };

        //drawFaceNPR(data, depth, texture, width, height, imgWidth, imgHeight, nprShade, nprSize,  pt1, pt2, pt3 );
        drawFaceNPRPerPixel(data, depth, texture, width, height, imgWidth, imgHeight, nprShade, nprSize, lightDir,  pt1, pt2, pt3 );
    }

    nm.calculateEdge();
    

    vector<kmVec4> lightVer;
    for(int i = 0; i < li.vertices.size(); i++) {
        kmVec4 out;
        kmVec4 temp;
        kmVec3 vt = li.vertices[i];
        kmVec4Fill(&temp, vt.x, vt.y, vt.z, 1);

        kmVec4 out2;
        kmVec4Transform(&out2, &temp, &lightViewMat);
        lightVer.push_back(out2);
    }

    for(int i=0; i < li.edges.size(); i++) {
        int a = li.edges[i].a;
        int b = li.edges[i].b;
        
        float apx = lightVer[a].x;
        float apy = lightVer[a].y;
        float aw = lightVer[a].w;

        float bpx = lightVer[b].x;
        float bpy = lightVer[b].y;
        float bw = lightVer[b].w;
        
        drawLine(data, width, height, apx/aw*cx+cx, apy/aw*cy+cy, bpx/bw*cx+cx, bpy/bw*cy+cy, i);
    }

    
    for(int i=0; i< nm.edges.size(); i++) {
        int a = nm.edges[i].a;
        int b = nm.edges[i].b;
        
        float apx = nm.vertices[a].x;
        float apy = nm.vertices[a].y;
        float aw = nm.vertices[a].w;

        float bpx = nm.vertices[b].x;
        float bpy = nm.vertices[b].y;
        float bw = nm.vertices[b].w;
        
        drawLine(data, width, height, apx/aw*cx+cx, apy/aw*cy+cy, bpx/bw*cx+cx, bpy/bw*cy+cy, i);
    }
    //li.render();
}

void Camera::renderFaceWithTexture(CCSprite *sp, Mesh *m, unsigned char *data, float diff) {
    //m->update(diff);

    CCSize tsz = sp->getContentSize(); 
    //投影每个mesh的顶点 计算2d 空间坐标
    vector<kmVec4> npos;
    vector<kmVec3> nnormal;

    //each vertex color
    vector<kmVec3> color;

    //ModelView Matrix
    //Project Matrix 两个矩阵进行计算

    kmMat4 matrixPers;
    kmMat4 matrixLookup;
    kmMat4 matrixModel;

    kmMat4PerspectiveProjection(&matrixPers, 60, tsz.width/tsz.height, 0.2f, 30);


    int cx = tsz.width/2;
    int cy = tsz.height/2;

    kmVec3 eye, center, up;
    kmVec3Fill( &eye, position.x, position.y,  position.z);
    kmVec3Fill( &center, 0, 0, 0.0f );
    kmVec3Fill( &up, 0.0f, 1.0f, 0.0f);
    kmMat4LookAt(&matrixLookup, &eye, &center, &up);
    
    kmMat3 rot;
    kmMat3RotationQuaternion(&rot, &m->rotation);
    kmMat4RotationTranslation(&matrixModel, &rot, &m->position);

    printMat(&matrixPers);

    printMat(&matrixLookup);

    printMat(&matrixModel);


    kmMat4 tempMat;
    kmMat4Multiply(&tempMat,  &matrixLookup, &matrixModel);

    kmMat4 allMat;
    kmMat4Multiply(&allMat, &matrixPers, &tempMat);
    printMat(&allMat);


    //only rotation  no translation
    //世界坐标中 提取出旋转 矩阵应用到 normal 上面

    kmVec3 light = {2, 2, 2};

    //Model matrix
    for(int i=0;i < m->vertices.size(); i++) {
        kmVec4 out;
        kmVec4 temp;
        kmVec3 vt = m->vertices[i];
        kmVec4Fill(&temp, vt.x, vt.y, vt.z, 1);

        //kmVec4Transform(&out, &temp, &matrixLookup);



        kmVec4 out2;
        //kmVec4Transform(&out2, &out, &matrixPers);
        kmVec4Transform(&out2, &temp, &allMat);

        npos.push_back(out2);
    
        //世界坐标重新计算normal light 位置不变
        kmVec3 nn;
        kmVec3TransformNormal(&nn, &m->normal[i], &matrixModel);
        nnormal.push_back(nn);


        kmVec3 worldPosition;
        kmVec3Transform(&worldPosition, &vt, &matrixModel);
        
        //diff = 
        kmVec3 tempRes;
        kmVec3 lightDir;
        kmVec3Subtract(&tempRes, &light, &worldPosition);
        kmVec3Normalize(&lightDir, &tempRes);
        
        //cosin  光线方向 和 normal 方向
        kmVec3Normalize(&nn, &nn);
        float cosTheta = kmVec3Dot(&lightDir, &nn);
        cosTheta = std::max(0.0f, cosTheta);

        //CCLog("color is %f", cosTheta);
        color.push_back({cosTheta, cosTheta, cosTheta});
    }
    


    memset(data, 0, (tsz.width*tsz.height*4));
    memset(depth, 0, width*height*sizeof(float));

    //clear buffer
    //CCLog("start %f %f", tsz.width, tsz.height);
    //数据的Y 方向似乎不对
    //图像数据 是 左上角 0 0 坐标的 因此 Y 方向就不对了

    //绘制 line 最简单的 绘线算法
    //旋转X 方向 反了 为什么呢?
    
    //
    for(int i=0; i < m->triangles.size(); i++) {
        int a = m->triangles[i].a;
        int b = m->triangles[i].b;
        int c = m->triangles[i].c;
        
        //CCLog("aa %d %d %d", a, b, c);
        float apx = npos[a].x;
        float apy = npos[a].y;
        float apz = npos[a].z;
        float aw = npos[a].w;

        float bpx = npos[b].x;
        float bpy = npos[b].y;
        float bpz = npos[b].z;
        float bw = npos[b].w;
        
        float cpx = npos[c].x;
        float cpy = npos[c].y;
        float cpz = npos[c].z;
        float cw = npos[c].w;
    
        //CCLog("a b c %f %f %f  %f %f %f  %f %f %f", apx, apy, aw, bpx, bpy, bw, cpx, cpy, cw);
        //根据 zbuffer数据 更新depth数据
        //drawFaceWithLight(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, color[a], bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, color[b], cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, color[c]);
        drawFaceWithTexture(data, depth, texture, tsz.width, tsz.height, imgWidth, imgHeight, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, m->textureCoord[a], bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, m->textureCoord[b], cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, m->textureCoord[c]);

    }


    

    //CCLog("end");

    CCTexture2D *oldTex = sp->getTexture();
    //if(oldTex)
    oldTex->release();
    //CCLog("remove old Texture");

    CCTexture2D *tex = new CCTexture2D();
    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, tsz.width, tsz.height, CCSizeMake(tsz.width, tsz.height));
    //sp->setFlipY(true);
    sp->setTexture(tex);
    
    //CCLog("set new Texture");

    //调整纹理 Y 方向 纹理坐标参数
}
