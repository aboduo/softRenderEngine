#include "util.h"
#include <vector>

using namespace cocos2d;

void putpixel(unsigned char *data, int width , int height, int x, int y) {
    x = std::max(std::min(width-1, x), 0);
    y = std::max(std::min(height-1, y), 0);
    int id = y*width+x;
    data[id*4+0] = 255;
    data[id*4+1] = 255;
    data[id*4+2] = 255;
    data[id*4+3] = 255;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

//扫描线fill 算法
void drawTriangle(unsigned char *data, int width, int height, int x1, int y1, int x2, int y2, int x3, int y3){
    CCLog("drawTriangle %d %d %d %d %d %d", x1, y1, x2, y2, x3, y3);
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

    CCLog("drawTriangle2 %d %d %d %d %d %d", x1, y1, x2, y2, x3, y3 );
    //CCLog("%f %f", x2-x1, y2-y1); 
    
    float dx_far = (x3-x1)*1.0f/(y3-y1+1);
    float dx_upper = (x2-x1)*1.0f/(y2-y1+1);
    float dx_low = (x3-x2)*1.0f/(y3-y2+1);

    float xf = x1;
    float xt = x1+dx_upper;
    
    float maxY = y3 > height-1? height-1: y3;


    CCLog("dx dy %f %f %f", dx_far, dx_upper, dx_low);
    CCLog("xf xt %f %f", xf, xt);
    for(int y=y1; y <= maxY; y++) {
        
        float minX = xf > 0 ? xf: 0;
        float maxX = xt < width-1? xt: width-1;
        for(int x=minX; x <= maxX; x++) {
            putpixel(data, width, height, x, y);
        }
        
        maxX = xf < width-1? xf: width-1;
        minX = xt > 0? xt: 0;

        for(int x=maxX; x >= minX; x--) {
            putpixel(data, width, height, x, y);
        }

        xf += dx_far;
        if(y < y2) {
            xt += dx_upper;
        }else {
            xt += dx_low;
        }
    }
}



//c color 
void drawLine(unsigned char *data, int width, int height, int x1, int y1, int x2, int y2, int c) {
  int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
  CCLog("draw Line %d %d %d %d", x1, y1, x2, y2);
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
    putpixel(data, width, height, x, y);
    
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
      putpixel(data, width, height, x, y);
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
    putpixel(data, width, height, x, y);
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
      putpixel(data, width, height, x, y);
    }
  }
}
