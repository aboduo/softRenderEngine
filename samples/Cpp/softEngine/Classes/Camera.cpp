#include "Camera.h"
#include "kazmath/mat4.h"
#include "kazmath/vec4.h"
#include "kazmath/quaternion.h"
#include "util.h"


using namespace std;


void printMat(kmMat4 *mat) {
    printf("mat \n");
    for(int i = 0; i < 16; i++) {
        printf("%f ", mat->mat[(i%4)*4+i/4]);
        if(i %4 == 3) {
            printf("\n");
        }
    }
    printf("\n");
}

void Camera::update(float diff) {
}


void Camera::renderFace(CCSprite *sp, Mesh *m, unsigned char *data, float diff) {
    m->update(diff);

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
    kmVec3Fill( &eye, 0, 0,  5);
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
    CCLog("start %f %f", tsz.width, tsz.height);
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

        CCLog("axy bxy %f %f %f %f %f %f", apx, apy, aw, bpx, bpy, bw);
        //根据角度绘制x y 方向角度绘制线
        drawLine(data, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, apy/aw*tsz.height/2+cy, bpx/bw*tsz.width/2+cx, bpy/bw*tsz.height/2+cy, 1);
    }
    */
    
    for(int i=0; i < m->triangles.size(); i++) {
        int a = m->triangles[i].a;
        int b = m->triangles[i].b;
        int c = m->triangles[i].c;
        
        CCLog("aa %d %d %d", a, b, c);
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
    
        CCLog("a b c %f %f %f  %f %f %f  %f %f %f", apx, apy, aw, bpx, bpy, bw, cpx, cpy, cw);
        //根据 zbuffer数据 更新depth数据
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, tsz.height-(apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, tsz.height-(bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, tsz.height-(cpy/cw*tsz.height/2+cy), cpz/cw, i);
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
        drawFace(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
    }


    

    CCLog("end");

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
    kmVec3Fill( &eye, 0, 0,  5);
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
    CCLog("start %f %f", tsz.width, tsz.height);
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

        CCLog("axy bxy %f %f %f %f %f %f", apx, apy, aw, bpx, bpy, bw);
        //根据角度绘制x y 方向角度绘制线
        drawLine(data, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, apy/aw*tsz.height/2+cy, bpx/bw*tsz.width/2+cx, bpy/bw*tsz.height/2+cy, i);
    }



    CCLog("end");

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
    CCLog("start %f %f", tsz.width, tsz.height);
    //数据的Y 方向似乎不对
    //图像数据 是 左上角 0 0 坐标的 因此 Y 方向就不对了

    //旋转X 方向 反了 为什么呢?
    for(int i=0; i < npos.size(); i++) {
        CCLog("over pos %f %f %f %f", npos[i].x, npos[i].y, npos[i].z, npos[i].w);
        CCLog("depth %f", npos[i].z/npos[i].w);

        float px = npos[i].x;
        float py = npos[i].y;
        float w = npos[i].w;

        //缩放一下尺寸
        int vx = std::min(std::max(0.0f, (px/w*tsz.width/2+cx)), tsz.width-1.0f);
        //Y 方向反转一下
        int vy = std::min(std::max(0.0f, (py/w*tsz.height/2+cy)), tsz.height-1.0f);
        
        CCLog("vx vy %d %d", vx, vy);
        
        data[int(vy*tsz.width+vx)*4+0] = 255; 
        data[int(vy*tsz.width+vx)*4+1] = 255; 
        data[int(vy*tsz.width+vx)*4+2] = 255; 
        data[int(vy*tsz.width+vx)*4+3] = 255; 
    }

    CCLog("end");

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



void Camera::renderFaceWithLight(CCSprite *sp, Mesh *m, unsigned char *data, float diff) {
    m->update(diff);

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
    kmVec3Fill( &eye, 0, 0,  5);
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

        CCLog("color is %f", cosTheta);
        color.push_back({cosTheta, cosTheta, cosTheta});
    }
    


    memset(data, 0, (tsz.width*tsz.height*4));
    memset(depth, 0, width*height*sizeof(float));

    //clear buffer
    CCLog("start %f %f", tsz.width, tsz.height);
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

        CCLog("axy bxy %f %f %f %f %f %f", apx, apy, aw, bpx, bpy, bw);
        //根据角度绘制x y 方向角度绘制线
        drawLine(data, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, apy/aw*tsz.height/2+cy, bpx/bw*tsz.width/2+cx, bpy/bw*tsz.height/2+cy, 1);
    }
    */
    
    for(int i=0; i < m->triangles.size(); i++) {
        int a = m->triangles[i].a;
        int b = m->triangles[i].b;
        int c = m->triangles[i].c;
        
        CCLog("aa %d %d %d", a, b, c);
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
    
        CCLog("a b c %f %f %f  %f %f %f  %f %f %f", apx, apy, aw, bpx, bpy, bw, cpx, cpy, cw);
        //根据 zbuffer数据 更新depth数据
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, tsz.height-(apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, tsz.height-(bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, tsz.height-(cpy/cw*tsz.height/2+cy), cpz/cw, i);
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
        //drawFace(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
        
        drawFaceWithLight(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, color[a], bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, color[b], cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, color[c]);

    }


    

    CCLog("end");

    CCTexture2D *oldTex = sp->getTexture();
    //if(oldTex)
    oldTex->release();
    CCLog("remove old Texture");

    CCTexture2D *tex = new CCTexture2D();
    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, tsz.width, tsz.height, CCSizeMake(tsz.width, tsz.height));
    //sp->setFlipY(true);
    sp->setTexture(tex);
    
    CCLog("set new Texture");


    //调整纹理 Y 方向 纹理坐标参数
    //sp->setFlipY(false);
    //float sca = sp->getScaleX();
    //sp->setScaleY(-sca);
}


void Camera::renderFaceWithTexture(CCSprite *sp, Mesh *m, unsigned char *data, float diff) {
    m->update(diff);

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
    kmVec3Fill( &eye, 0, 0,  5);
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

        CCLog("color is %f", cosTheta);
        color.push_back({cosTheta, cosTheta, cosTheta});
    }
    


    memset(data, 0, (tsz.width*tsz.height*4));
    memset(depth, 0, width*height*sizeof(float));

    //clear buffer
    CCLog("start %f %f", tsz.width, tsz.height);
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

        CCLog("axy bxy %f %f %f %f %f %f", apx, apy, aw, bpx, bpy, bw);
        //根据角度绘制x y 方向角度绘制线
        drawLine(data, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, apy/aw*tsz.height/2+cy, bpx/bw*tsz.width/2+cx, bpy/bw*tsz.height/2+cy, 1);
    }
    */
    
    for(int i=0; i < m->triangles.size(); i++) {
        int a = m->triangles[i].a;
        int b = m->triangles[i].b;
        int c = m->triangles[i].c;
        
        CCLog("aa %d %d %d", a, b, c);
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
    
        CCLog("a b c %f %f %f  %f %f %f  %f %f %f", apx, apy, aw, bpx, bpy, bw, cpx, cpy, cw);
        //根据 zbuffer数据 更新depth数据
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, tsz.height-(apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, tsz.height-(bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, tsz.height-(cpy/cw*tsz.height/2+cy), cpz/cw, i);
        //drawTriangle(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
        //drawFace(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, i);
        
        drawFaceWithLight(data, depth, tsz.width, tsz.height, apx/aw*tsz.width/2+cx, (apy/aw*tsz.height/2+cy), apz/aw, color[a], bpx/bw*tsz.width/2+cx, (bpy/bw*tsz.height/2+cy), bpz/bw, color[b], cpx/cw*tsz.width/2+cx, (cpy/cw*tsz.height/2+cy), cpz/cw, color[c]);

    }


    

    CCLog("end");

    CCTexture2D *oldTex = sp->getTexture();
    //if(oldTex)
    oldTex->release();
    CCLog("remove old Texture");

    CCTexture2D *tex = new CCTexture2D();
    tex->initWithData(data, kCCTexture2DPixelFormat_RGBA8888, tsz.width, tsz.height, CCSizeMake(tsz.width, tsz.height));
    //sp->setFlipY(true);
    sp->setTexture(tex);
    
    CCLog("set new Texture");


    //调整纹理 Y 方向 纹理坐标参数
    //sp->setFlipY(false);
    //float sca = sp->getScaleX();
    //sp->setScaleY(-sca);
}
