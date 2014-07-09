#ifndef __OVER_LAY__
#define __OVER_LAY__
#include "cocos2d.h"
#include <vector>
#include "kazmath/vec4.h"
#include "Mesh.h"

using namespace cocos2d;
using namespace std;

class OverLay : public CCLayer{
public:
    int width;
    int height;

    virtual bool init();  
    CREATE_FUNC(OverLay);

    //vector<CCLabel*> label;

    //根据传入的定点位置 投影到屏幕上的位置来初始化label
    //same color for same face
    
    void renderLabe(int i, kmVec4 p, int c) {

        static int color[][3] = {
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

        char buf[512];
        sprintf(buf, "%d", i);
        CCLabelTTF *lab = CCLabelTTF::create(buf, "", 10);
        addChild(lab);
        int rx = random()%50; 
        int ry = random()%1;
        lab->setPosition(ccp(p.x*width+200+rx, p.y*height+200+ry));
        int id = c%9;

        lab->setColor(ccc3(color[id][0], color[id][1], color[id][2]));
    }

    void initLabel(vector<kmVec4> &pos, vector<Triangle> &triangles) {


        for(int i = 0; i < triangles.size(); i++) {
            Triangle tri = triangles[i];
            kmVec4 p = pos[tri.a];
            renderLabe(tri.a, p, i);        

            p = pos[tri.b];
            renderLabe(tri.b, p, i);        

            p = pos[tri.c];
            renderLabe(tri.c, p, i);        
        }
        /*
        for(int i = 0; i < pos.size(); i++) {
            sprintf(buf, "%d", i);
            CCLabelTTF *lab = CCLabelTTF::create(buf, "", 10);
            addChild(lab);
            int rx = random()%50; 
            int ry = random()%50;
            lab->setPosition(ccp(p.x*width*2+150+rx, p.y*height*2+150+ry));
        }
        */
    }
};
#endif
