#include "util.h"
#include <vector>

using namespace cocos2d;

static void printMat(kmMat4 *mat) {
    //printf("mat \n");
    for(int i = 0; i < 16; i++) {
        //printf("%f ", mat->mat[(i%4)*4+i/4]);
        if(i %4 == 3) {
            //printf("\n");
        }
    }
    //printf("\n");
}

void printVec3(kmVec3 p) {
    //CCLog("%f %f %f", p.x, p.y, p.z);
}
int color[][3] = {
    {255, 0, 0},
    {255, 100, 0},
    {255, 0, 100},
    {255, 100, 100},

    {0, 255, 0},
    {100, 255, 0},
    {0, 255, 100},
    {100, 255, 100},

    {0, 0, 255},
    {100, 0, 255},
    {0, 100, 255},
    {100, 100, 255},
};

void putpixel(unsigned char *data, int width , int height, int x, int y, int col) {
    x = std::max(std::min(width-1, x), 0);
    y = std::max(std::min(height-1, y), 0);
    int id = y*width+x;
    int r, g, b;
    r = g = b = 0;
    int idx = col%12;
    r = color[idx][0];
    g = color[idx][1];
    b = color[idx][2];

    /*
    if(col % 12 == 0) {
        r = 255;
    } else if(col % 3 == 1) {
        g = 255;
    } else {
        b = 255;
    }
    */

    data[id*4+0] = r;
    data[id*4+1] = g;
    data[id*4+2] = b;
    data[id*4+3] = 255;
}

template <typename T>
void swap(T *a, T *b) {
    T temp = *a;
    *a = *b;
    *b = temp;
}


//扫描线fill 算法
void drawTriangle(unsigned char *data, float *depth, int width, int height, int x1, int y1, float z1, int x2, int y2, float z2, int x3, int y3, float z3, int col){
    //CCLog("drawTriangle %d %d %f %d %d %f %d %d %f", x1, y1, z1, x2, y2, z2, x3, y3, z3);
    int temp;
    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }
    if(y2 > y3) {
        swap(&x2, &x3);
        swap(&y2, &y3);
    }
    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }

    //CCLog("drawTriangle2 %d %d %d %d %d %d", x1, y1, x2, y2, x3, y3 );
    ////CCLog("%f %f", x2-x1, y2-y1); 
    
    float dx_far = (x3-x1)*1.0f/(y3-y1+1);
    float dx_upper = (x2-x1)*1.0f/(y2-y1+1);
    float dx_low = (x3-x2)*1.0f/(y3-y2+1);
    

    float dz_far = (z3-z1)*1.0f/(y3-y1+1);
    float dz_upper = (z2-z1)*1.0f/(y2-y1+1);
    float dz_low = (z3-z2)*1.0f/(y3-y2+1);

    
    float xf = x1;
    float xt = x1+dx_upper;

    float zf = z1;
    float zt = z1+dz_upper;

    
    float maxY = y3 > height-1? height-1: y3;


    //CCLog("dx dy %f %f %f", dx_far, dx_upper, dx_low);
    //CCLog("xf xt %f %f", xf, xt);
    //线性插值得到 z 值
    int vx1 = x3-x1;
    int vy1 = y3-y1;
    int vx2 = x2-x1;
    int vy2 = y2-y1;

    //float delta = vx1*vy2-vx2*vy1;

    for(int y=y1; y <= maxY; y++) {

        float minX = xf > 0 ? xf: 0;
        float maxX = xt < width-1? xt: width-1;
        float sz = zf;
        float ez = zt;
        for(int x=minX; x <= maxX; x++) {
            /*
            float ax, by;
            float dx = x-x1;
            float dy = y-y1;

            float sx = dx*vy2-vx2*dy;
            float sy = vx1*dy-dx*vy1;

            ax = sx/delta;
            by = sy/delta;
            float newDepth = ax*(z3-z1)+by*(z2-z1)+z1;
            */

            float newDepth = sz + (ez-sz)*(x-minX)/(maxX-minX+1);
            float oldDepth = depth[y*width+x];

            //靠近 屏幕更近的点
            if(oldDepth == 0 ||newDepth < oldDepth) {
                depth[y*width+x] = newDepth;
                putpixel(data, width, height, x, y, col);
            }
        }
        
        maxX = xf < width-1? xf: width-1;
        minX = xt > 0? xt: 0;

        sz = zt;
        ez = zf;

        for(int x=maxX; x >= minX; x--) {
            /*
            float ax, by;
            float dx = x-x1;
            float dy = y-y1;

            float sx = dx*vy2-vx2*dy;
            float sy = vx1*dy-dx*vy1;

            ax = sx/delta;
            by = sy/delta;
            float newDepth = ax*(z3-z1)+by*(z2-z1)+z1;
            */
            float newDepth = sz + (ez-sz)*(x-minX)/(maxX-minX+1);

            float oldDepth = depth[y*width+x];
            if(oldDepth == 0 || newDepth < oldDepth) {
                depth[y*width+x] = newDepth;
                putpixel(data, width, height, x, y, col);
            }

            //putpixel(data, width, height, x, y, col);
        }

        xf += dx_far;
        zf += dz_far;
        if(y < y2) {
            xt += dx_upper;
            zt += dz_upper;
        }else {
            xt += dx_low;
            zt += dz_low;
        }
    }
}



//c color 
void drawLine(unsigned char *data, int width, int height, int x1, int y1, int x2, int y2, int col) {
  int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
  //CCLog("draw Line %d %d %d %d", x1, y1, x2, y2);
  dx = x2 - x1;
  dy = y2 - y1;
  dx1 = fabs(dx);
  dy1 = fabs(dy);
  px = 2 * dy1 - dx1;
  py = 2 * dx1 - dy1;
  if (dy1 <= dx1) {
    if (dx >= 0) {
      x = x1;
      y = y1;
      xe = x2;
    } else {
      x = x2;
      y = y2;
      xe = x1;
    }
    putpixel(data, width, height, x, y, col);
    
    for (i = 0; x < xe; i++) {
      x = x + 1;
      if (px < 0) {
        px = px + 2 * dy1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          y = y + 1;
        } else {
          y = y - 1;
        }
        px = px + 2 * (dy1 - dx1);
      }
      //delay(0);
      putpixel(data, width, height, x, y, col);
    }
  } else {
    if (dy >= 0) {
      x = x1;
      y = y1;
      ye = y2;
    } else {
      x = x2;
      y = y2;
      ye = y1;
    }
    putpixel(data, width, height, x, y, col);
    for (i = 0; y < ye; i++) {
      y = y + 1;
      if (py <= 0) {
        py = py + 2 * dx1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
          x = x + 1;
        } else {
          x = x - 1;
        }
        py = py + 2 * (dx1 - dy1);
      }
      putpixel(data, width, height, x, y, col);
    }
  }
}

float clamp(float value, float min=0, float max=1) {
    return std::max(min, std::min(max, value));
}

float interpolate(float min, float max, float gradient) {
    return min+(max-min)*clamp(gradient);
}

//void putpixel(unsigned char *data, int width , int height, int x, int y, int col) {
void processScanLine(unsigned char *data, float *depth, int width, int height, int y, kmVec3 pa, kmVec3 pb, kmVec3 pc, kmVec3 pd, int col) {
    float gradient1 = pa.y != pb.y ? (y-pa.y)/(pb.y-pa.y) : 1;
    float gradient2 = pc.y != pd.y? (y-pc.y)/(pd.y-pc.y) : 1;
    
    int sx = (int)interpolate(pa.x, pb.x, gradient1);
    int st = (int)interpolate(pc.x, pd.x, gradient2);
    
    float zx = interpolate(pa.z, pb.z, gradient1);
    float ze = interpolate(pc.z, pd.z, gradient2);
    //float deltaZ = (ze-zx)/(st-sx);

    for(int x=sx; x < st; x++) {
        float gradientZ = (float)(x-sx)/(st-sx);
        float newDepth = interpolate(zx, ze, gradientZ);
        float oldDepth = depth[y*width+x];
        if(oldDepth == 0 || newDepth < oldDepth) {
            depth[y*width+x] = newDepth;
            putpixel(data, width, height, x, y, col);
        }
    }
}



void drawFace(unsigned char *data, float *depth, int width, int height, int x1, int y1, float z1, int x2, int y2, float z2,  int x3, int y3, float z3,  int col) {
    //CCLog("drawFace %d %d %f %d %d %f %d %d %f", x1, y1, z1, x2, y2, z2, x3, y3, z3);

    //忘记交换z 值 导致depth 错乱了
    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
        swap(&z1, &z2);
    }
    if(y2 > y3) {
        swap(&x2, &x3);
        swap(&y2, &y3);
        swap(&z2, &z3);
    }
    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
        swap(&z1, &z2);
    }
    
    //CCLog("x 1 2 3 is %d %d %d %d %d %d", x1, y1, x2, y2, x3, y3);
    
    float dP1P2 = 0;
    float dP1P3 = 0;
    bool right = false;
    bool left = false;

    //y2 -y1 == 0 bug
    if(y2 - y1 > 0) {
        dP1P2 = (x2-x1)*1.0f/(y2-y1);
    //无穷大
    }else if(x2 > x1){
        right = true;
    //无穷小
    }else {
        left = true;
    }
    
    if(y3-y1 > 0) {
        dP1P3 = (x3-x1)*1.0f/(y3-y1);
    }else {
        dP1P3 = 0;
    }
    
    //CCLog("dp1p2 %f %f", dP1P2, dP1P3);

    kmVec3 p1 = {x1, y1, z1};
    kmVec3 p2 = {x2, y2, z2};
    kmVec3 p3 = {x3, y3, z3};
    

    if(right || (!left && dP1P2 > dP1P3)) {
        for(int y=y1; y <= y3; y++) {
            if(y < y2) {
                processScanLine(data, depth, width, height, y, p1, p3, p1, p2, col);
            }else {
                processScanLine(data, depth, width, height, y, p1, p3, p2, p3, col);
            }
        }
    } else {
        //p2 on left
        for(int y=y1; y <= y3; y++) {
            if(y < y2) {
                processScanLine(data, depth, width, height, y, p1, p2, p1, p3, col);
            }else {
                processScanLine(data, depth, width, height, y, p2, p3, p1, p3, col);
            }
        }
    }
}

void putpixelWithLight(unsigned char *data, int width , int height, int x, int y, float col) {
    x = std::max(std::min(width-1, x), 0);
    y = std::max(std::min(height-1, y), 0);
    int id = y*width+x;
    unsigned int r, g, b;
    //加一些环境光照
    r = g = b = 255*col+5;

    data[id*4+0] = r;
    data[id*4+1] = g;
    data[id*4+2] = b;
    data[id*4+3] = 255;
}


void processScanLineWithLight(unsigned char *data, float *depth, int width, int height, int y, kmVec3 pa, kmVec3 ca, kmVec3 pb, kmVec3 cb, kmVec3 pc, kmVec3 cc,  kmVec3 pd, kmVec3 cd) {
    float gradient1 = pa.y != pb.y ? (y-pa.y)/(pb.y-pa.y) : 1;
    float gradient2 = pc.y != pd.y? (y-pc.y)/(pd.y-pc.y) : 1;
    
    int sx = (int)interpolate(pa.x, pb.x, gradient1);
    int st = (int)interpolate(pc.x, pd.x, gradient2);
    
    float zx = interpolate(pa.z, pb.z, gradient1);
    float ze = interpolate(pc.z, pd.z, gradient2);
    //float deltaZ = (ze-zx)/(st-sx);

    float lightS = interpolate(ca.x, cb.x, gradient1);
    float lightE = interpolate(cc.x, cd.x, gradient2);

    for(int x=sx; x < st; x++) {
        float gradientZ = (float)(x-sx)/(st-sx);
        float newDepth = interpolate(zx, ze, gradientZ);
        float oldDepth = depth[y*width+x];
        if(oldDepth == 0 || newDepth < oldDepth) {
            depth[y*width+x] = newDepth;

            float light = interpolate(lightS, lightE, gradientZ);
            putpixelWithLight(data, width, height, x, y, light);
        }
    }
}

void drawFaceWithLight(unsigned char *data, float *depth, int width, int height, int x1, int y1, float z1, kmVec3 col1, int x2, int y2, float z2, kmVec3 col2,  int x3, int y3, float z3,  kmVec3 col3){
    //CCLog("drawFace %d %d %f %d %d %f %d %d %f", x1, y1, z1, x2, y2, z2, x3, y3, z3);
    //CCLog("color %f %f %f", col1.x, col2.x, col3.x);

    //忘记交换z 值 导致depth 错乱了
    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
        swap(&z1, &z2);
        swap(&col1, &col2);
    }
    if(y2 > y3) {
        swap(&x2, &x3);
        swap(&y2, &y3);
        swap(&z2, &z3);
        swap(&col2, &col3);
    }

    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
        swap(&z1, &z2);
        swap(&col1, &col2);
    }
    
    //CCLog("x 1 2 3 is %d %d %d %d %d %d", x1, y1, x2, y2, x3, y3);
    
    float dP1P2 = 0;
    float dP1P3 = 0;
    bool right = false;
    bool left = false;

    //y2 -y1 == 0 bug
    if(y2 - y1 > 0) {
        dP1P2 = (x2-x1)*1.0f/(y2-y1);
    //无穷大
    }else if(x2 > x1){
        right = true;
    //无穷小
    }else {
        left = true;
    }
    
    if(y3-y1 > 0) {
        dP1P3 = (x3-x1)*1.0f/(y3-y1);
    }else {
        dP1P3 = 0;
    }
    
    //CCLog("dp1p2 %f %f", dP1P2, dP1P3);

    kmVec3 p1 = {x1, y1, z1};
    kmVec3 p2 = {x2, y2, z2};
    kmVec3 p3 = {x3, y3, z3};
    

    if(right || (!left && dP1P2 > dP1P3)) {
        for(int y=y1; y <= y3; y++) {
            if(y < y2) {
                processScanLineWithLight(data, depth, width, height, y, p1, col1, p3, col3,  p1, col1,  p2, col2);
            }else {
                processScanLineWithLight(data, depth, width, height, y, p1, col1, p3, col3,  p2, col2,  p3, col3);
            }
        }
    } else {
        //p2 on left
        for(int y=y1; y <= y3; y++) {
            if(y < y2) {
                processScanLineWithLight(data, depth, width, height, y, p1, col1, p2, col2, p1, col1, p3, col3);
            }else {
                processScanLineWithLight(data, depth, width, height, y, p2, col2, p3, col3, p1, col1, p3, col3);
            }
        }
    }
}


void putpixelWithColor(unsigned char *data, int width , int height, int x, int y, int r, int g, int b, int a) {
    x = std::max(std::min(width-1, x), 0);
    y = std::max(std::min(height-1, y), 0);
    int id = y*width+x;

    data[id*4+0] = r;
    data[id*4+1] = g;
    data[id*4+2] = b;
    data[id*4+3] = a;
}

void processScanLineWithTexture(unsigned char *data, float *depth, unsigned char *texture, int width, int height, int imgWidth, int imgHeight, int y, kmVec3 pa, kmVec2 texa, kmVec3 pb, kmVec2 texb, kmVec3 pc, kmVec2 texc,  kmVec3 pd, kmVec2 texd) {
    float gradient1 = pa.y != pb.y ? (y-pa.y)/(pb.y-pa.y) : 1;
    float gradient2 = pc.y != pd.y? (y-pc.y)/(pd.y-pc.y) : 1;
    
    int sx = (int)interpolate(pa.x, pb.x, gradient1);
    int st = (int)interpolate(pc.x, pd.x, gradient2);
    
    float zx = interpolate(pa.z, pb.z, gradient1);
    float ze = interpolate(pc.z, pd.z, gradient2);
    //float deltaZ = (ze-zx)/(st-sx);

    //float lightS = interpolate(ca.x, cb.x, gradient1);
    //float lightE = interpolate(cc.x, cd.x, gradient2);

    float texXS = interpolate(texa.x, texb.x, gradient1);
    float texXE = interpolate(texc.x, texd.x, gradient2);
    float texYS = interpolate(texa.y, texb.y, gradient1);
    float texYE = interpolate(texc.y, texd.y, gradient2);

    ////CCLog("line is %d %d %d", y, sx, st);
    ////CCLog("tex coord %f %f %f %f", texXS, texXE, texYS, texYE);

    ////CCLog("range %d %d %d %d", (int)(texXS*imgWidth), int(texXE*imgWidth), int(texYS*imgHeight), int(texYE*imgHeight));

    for(int x=sx; x < st; x++) {
        float gradientZ = (float)(x-sx)/(st-sx);
        float newDepth = interpolate(zx, ze, gradientZ);
        float oldDepth = depth[y*width+x];
        if(oldDepth == 0 || newDepth < oldDepth) {
            depth[y*width+x] = newDepth;
            
            float texX = interpolate(texXS, texXE, gradientZ);
            float texY = interpolate(texYS, texYE, gradientZ);

            //float light = interpolate(lightS, lightE, gradientZ);
            texX = std::max(std::min(1.0f, texX), 0.0f);
            texY = std::max(std::min(1.0f, texY), 0.0f);
            int thei = (int)(texY*(imgHeight-1));
            int twid = (int)(texX*(imgWidth-1));

            int tid = (int)(thei*imgWidth+twid);
            ////CCLog("tid is %d %f %f %d %d", tid, texX, texY, twid, thei);

            int r = texture[tid*4+0];
            int g = texture[tid*4+1];
            int b = texture[tid*4+2];
            int a = texture[tid*4+3];

            ////CCLog("rgba %d %d %d %d", r, g, b, a);

            //putpixelWithColor(data, width, height, x, y, 255, 255, 255, 255);
            putpixelWithColor(data, width, height, x, y, r,  g, b, a);
        }
    }
}

//纹理Y坐标 和 采样数据需要变化一下
void drawFaceWithTexture(unsigned char *data, float *depth, unsigned char *texture, int width, int height, int imgWidth, int imgHeight, int x1, int y1, float z1, kmVec2 tex1, int x2, int y2, float z2, kmVec2 tex2,  int x3, int y3, float z3,  kmVec2 tex3){

    //CCLog("drawFace With Texture %d %d %f %d %d %f %d %d %f", x1, y1, z1, x2, y2, z2, x3, y3, z3);
    //CCLog("texture coord %f %f %f %f %f %f", tex1.x, tex1.y, tex2.x, tex2.y, tex3.x, tex3.y);
    //CCLog("image size %d %d", imgWidth, imgHeight);
    //调整y 坐标
    tex1.y = 1-tex1.y;
    tex2.y = 1-tex2.y;
    tex3.y = 1-tex3.y;

    //忘记交换z 值 导致depth 错乱了
    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
        swap(&z1, &z2);
        swap(&tex1, &tex2);
    }
    if(y2 > y3) {
        swap(&x2, &x3);
        swap(&y2, &y3);
        swap(&z2, &z3);
        swap(&tex2, &tex3);
    }

    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
        swap(&z1, &z2);
        swap(&tex1, &tex2);
    }
    
    //CCLog("x 1 2 3 is %d %d %d %d %d %d", x1, y1, x2, y2, x3, y3);
    
    float dP1P2 = 0;
    float dP1P3 = 0;
    bool right = false;
    bool left = false;

    //y2 -y1 == 0 bug
    if(y2 - y1 > 0) {
        dP1P2 = (x2-x1)*1.0f/(y2-y1);
    //无穷大
    }else if(x2 > x1){
        right = true;
    //无穷小
    }else {
        left = true;
    }
    
    if(y3-y1 > 0) {
        dP1P3 = (x3-x1)*1.0f/(y3-y1);
    }else {
        dP1P3 = 0;
    }
    
    //CCLog("dp1p2 %f %f", dP1P2, dP1P3);

    kmVec3 p1 = {x1, y1, z1};
    kmVec3 p2 = {x2, y2, z2};
    kmVec3 p3 = {x3, y3, z3};
    

    //CCLog("scan line");
    if(right || (!left && dP1P2 > dP1P3)) {
        for(int y=y1; y <= y3; y++) {
            if(y < y2) {
                processScanLineWithTexture(data, depth, texture, width, height, imgWidth, imgHeight, y, p1, tex1, p3, tex3,  p1, tex1,  p2, tex2);
            }else {
                processScanLineWithTexture(data, depth, texture, width, height, imgWidth, imgHeight, y, p1, tex1, p3, tex3,  p2, tex2,  p3, tex3);
            }
        }
    } else {
        //p2 on left
        for(int y=y1; y <= y3; y++) {
            if(y < y2) {
                processScanLineWithTexture(data, depth, texture, width, height, imgWidth, imgHeight, y, p1, tex1, p2, tex2, p1, tex1, p3, tex3);
            }else {
                processScanLineWithTexture(data, depth, texture, width, height, imgWidth, imgHeight, y, p2, tex2, p3, tex3, p1, tex1, p3, tex3);
            }
        }
    }
}


//可以将lightDepth 渲染出来
//-1 1 范围 转换成 灰度图
static void processScanLineLightDepth(float *lightDepth, int width, int height, int y, kmVec3 pa, kmVec3 pb, kmVec3 pc, kmVec3 pd) {
    float gradient1 = pa.y != pb.y ? (y-pa.y)/(pb.y-pa.y) : 1;
    float gradient2 = pc.y != pd.y? (y-pc.y)/(pd.y-pc.y) : 1;
    
    int sx = (int)interpolate(pa.x, pb.x, gradient1);
    int st = (int)interpolate(pc.x, pd.x, gradient2);
    
    //从光源点深度值
    float zx = interpolate(pa.z, pb.z, gradient1);
    float ze = interpolate(pc.z, pd.z, gradient2);

    for(int x=sx; x < st; x++) {
        float gradientZ = (float)(x-sx)/(st-sx);
        float newDepth = interpolate(zx, ze, gradientZ);
        float oldDepth = lightDepth[y*width+x];
        if(oldDepth == 0 || newDepth < oldDepth) {
            lightDepth[y*width+x] = newDepth;
        }
    }
}

static kmVec3 interpolateVec3(kmVec3 pa, kmVec3 pb, float gradient1) {
    float tx = interpolate(pa.x, pb.x, gradient1);
    float ty = interpolate(pa.y, pb.y, gradient1);
    float tz = interpolate(pa.z, pb.z, gradient1);
    kmVec3 temp = {tx, ty, tz};
    return temp;
}
//screen position world position 
//lightWorldMatrix 
static void processScanLineShadow(unsigned char *data, float *depth, float *lightDepth, int width, int height, kmMat4 lightMatrix, int y, kmVec3 pa, kmVec3 wpa, kmVec3 pb, kmVec3 wpb, kmVec3 pc, kmVec3 wpc, kmVec3 pd, kmVec3 wpd) {
    float gradient1 = pa.y != pb.y ? (y-pa.y)/(pb.y-pa.y) : 1;
    float gradient2 = pc.y != pd.y? (y-pc.y)/(pd.y-pc.y) : 1;

    //interpolate light value

    ////CCLog("line %f %f %f %f %f %f %f %f", pa.x, pa.y, pb.x, pb.y, pc.x, pc.y, pd.x, pd.y);
    
    int sx = (int)interpolate(pa.x, pb.x, gradient1);
    int st = (int)interpolate(pc.x, pd.x, gradient2);
    //CCLog("start %d %d %d", sx, st, y);
    printVec3(wpa);
    printVec3(wpb);
    printVec3(wpc);
    printVec3(wpd);
    //sx = std::min(std::max(0, sx), width-1);
    //st = std::min(std::max(0, st), width-1);
    
    //从光源点深度值
    float zx = interpolate(pa.z, pb.z, gradient1);
    float ze = interpolate(pc.z, pd.z, gradient2);

    kmVec3 temp1, temp2;
    temp1 = interpolateVec3(wpa, wpb, gradient1);
    temp2 = interpolateVec3(wpc, wpd, gradient2);
    //printf("temp pos \n");
    printVec3(temp1);
    printVec3(temp2);

    int cx = width/2;
    int cy = height/2;

    int minX = std::min(std::max(0, sx), width-1);
    int maxX = std::min(std::max(0, st), width);

    for(int x=minX; x < maxX; x++) {
        float gradientZ = (float)(x-sx)/(st-sx);
        float newDepth = interpolate(zx, ze, gradientZ);
        float oldDepth = depth[y*width+x];
        if(newDepth < oldDepth) {
            depth[y*width+x] = newDepth;
            //float light = interpolate(lightS, lightE, gradientZ);

            //gradient Y gradient x 

            //putpixel(data, width, height, x, y, col);

            kmVec3 temp = interpolateVec3(temp1, temp2, gradientZ);
            //printf("worlPos %f %f %f\n", temp.x, temp.y, temp.z);

            kmVec4 temp4;
            kmVec4Fill(&temp4, temp.x, temp.y, temp.z, 1);
            kmVec4 out;
            //light Screen coordinate
            kmVec4Transform(&out, &temp4, &lightMatrix);
            //printf("out if %f %f %f %f\n", out.x, out.y, out.z, out.w);
            
            kmVec3 p1 = {out.x/out.w*cx+cx, out.y/out.w*cy+cy, out.z/out.w};
            printVec3(p1);
            //int xcoord = (int)(p1.x*cx+cx);
            //int ycoord = (int)(p1.y*cy+cy);
            int xcoord = (int)p1.x;
            int ycoord = (int)p1.y;

            //printf("xcoord %d %d\n", xcoord, ycoord);

            xcoord = std::max(std::min(width-1, xcoord), 0);
            ycoord = std::max(std::min(height-1, ycoord), 0);
            //获取光源深度缓冲区
            float lv = lightDepth[ycoord*width+xcoord];

            
            //printf("%d %d %.2f %.2f %d %d %.2f %.2f\n", x, y, oldDepth, newDepth, xcoord, ycoord, p1.z, lv);
            
            //和该点的光源深度进行比较
            //in shadow far away from some other face 
            //该点比 实际点的距离
            float dif = std::min(std::max(p1.z-lv, 0.0f)*20, 0.9f);
            //if(p1.z > lv) {
                putpixelWithLight(data, width, height, x, y, 0.9-dif);
            //}else {
            //    putpixelWithLight(data, width, height, x, y, 0.9);
            //}

            //putpixelWithLight(data, width, height, x, y, 0.9);

            //putpixel(data, width, height, x, y, 1);
        }
    }
    //printf("\n");
}

void swapVec3(kmVec3 *pa, kmVec3 *pb) {
    kmVec3 temp;
    temp.x = pa->x;
    temp.y = pa->y;
    temp.z = pa->z;

    pa->x = pb->x;
    pa->y = pb->y;
    pa->z = pb->z;

    pb->x = temp.x;
    pb->y = temp.y;
    pb->z = temp.z;
}

void drawShadow(unsigned char *data, float *depth, float *lightDepth, int width, int height, kmMat4 lightMatrix, kmVec3 p1, kmVec3 wp1,  kmVec3 p2, kmVec3 wp2,  kmVec3 p3, kmVec3 wp3){
    //CCLog("drawShadow");
    printVec3(p1);
    printVec3(p2);
    printVec3(p3);

    if(p1.y > p2.y) {
        swapVec3(&p1, &p2);
        swapVec3(&wp1, &wp2);
    }
    if(p2.y > p3.y) {
        swapVec3(&p2, &p3);
        swapVec3(&wp2, &wp3);
    }
    if(p1.y > p2.y) {
        swapVec3(&p1, &p2);
        swapVec3(&wp1, &wp2);
    }

    //CCLog("drawShadow2");
    printVec3(p1);
    printVec3(p2);
    printVec3(p3);


    float dP1P2 = 0;
    float dP1P3 = 0;
    bool right = false;
    bool left = false;

    //y2 -y1 == 0 bug
    if(p2.y - p1.y > 0) {
        dP1P2 = (p2.x-p1.x)*1.0f/(p2.y-p1.y);
    //无穷大
    }else if(p2.x > p1.x){
        right = true;
    //无穷小
    }else {
        left = true;
    }
    if(p3.y-p1.y > 0) {
        dP1P3 = (p3.x-p1.x)*1.0f/(p3.y-p1.y);
    }else {
        dP1P3 = 0;
    }

    
    int minY = std::min(std::max(0, (int)p1.y), height-1);
    int maxY = std::min(std::max(0, (int)p3.y), height-1);

    //CCLog("min max Y %d %d", minY, maxY);
    if(right || (!left && dP1P2 > dP1P3)) {
        for(int y=minY; y <= maxY; y++) {
            if(y < p2.y) {
                processScanLineShadow(data, depth, lightDepth, width, height, lightMatrix, y, p1, wp1, p3, wp3, p1, wp1, p2, wp2);
            }else {
                processScanLineShadow(data, depth, lightDepth, width, height, lightMatrix, y, p1, wp1, p3, wp3, p2, wp2, p3, wp3);
            }
        }
    } else {
        //p2 on left
        for(int y=minY; y <= maxY; y++) {
            if(y < p2.y) {
                processScanLineShadow(data, depth, lightDepth, width, height, lightMatrix, y, p1, wp1, p2, wp2, p1, wp1, p3, wp3);
            }else {
                processScanLineShadow(data, depth, lightDepth, width, height, lightMatrix, y, p2, wp2, p3, wp3, p1, wp1, p3, wp3);
            }
        }
    }
}


void drawLightDepth(float *lightDepth, int width, int height, int x1, int y1, float z1, int x2, int y2, float z2,  int x3, int y3, float z3) {
    //忘记交换z 值 导致depth 错乱了
    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
        swap(&z1, &z2);
    }
    if(y2 > y3) {
        swap(&x2, &x3);
        swap(&y2, &y3);
        swap(&z2, &z3);
    }

    if(y1 > y2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
        swap(&z1, &z2);
    }


    float dP1P2 = 0;
    float dP1P3 = 0;
    bool right = false;
    bool left = false;

    //y2 -y1 == 0 bug
    if(y2 - y1 > 0) {
        dP1P2 = (x2-x1)*1.0f/(y2-y1);
    //无穷大
    }else if(x2 > x1){
        right = true;
    //无穷小
    }else {
        left = true;
    }
    
    if(y3-y1 > 0) {
        dP1P3 = (x3-x1)*1.0f/(y3-y1);
    }else {
        dP1P3 = 0;
    }


    kmVec3 p1 = {x1, y1, z1};
    kmVec3 p2 = {x2, y2, z2};
    kmVec3 p3 = {x3, y3, z3};
    
    if(right || (!left && dP1P2 > dP1P3)) {
        for(int y=y1; y <= y3; y++) {
            if(y < y2) {
                processScanLineLightDepth(lightDepth, width, height, y, p1, p3, p1, p2);
            }else {
                processScanLineLightDepth(lightDepth, width, height, y, p1, p3, p2, p3);
            }
        }
    } else {
        //p2 on left
        for(int y=y1; y <= y3; y++) {
            if(y < y2) {
                processScanLineLightDepth(lightDepth, width, height, y, p1, p2, p1, p3);
            }else {
                processScanLineLightDepth(lightDepth, width, height, y, p2, p3, p1, p3);
            }
        }
    }
}
