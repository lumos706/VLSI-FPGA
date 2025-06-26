#ifndef FPGATILE_H
#define FPGATILE_H

#include <assert.h>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

class RRNode;

class FpgaTile{
public:
    FpgaTile(int r, int c, int W);
    virtual ~FpgaTile();

private:
    int x;
    int y;
    int W;

    // 指向相邻的FPGA块的指针
    FpgaTile *left;
    FpgaTile *right;
    FpgaTile *up;
    FpgaTile *down;

    map<int, RRNode *> logicPin;    // 逻辑块引脚
    vector<RRNode *> vWires;    // 垂直导线
    vector<RRNode *> hWires;    // 水平导线

    vector<RRNode *> rrNodes;   // 该FPGA块拥有的布线资源节点（导线或引脚）

public:
    int getX() { return x; }    // 获得FPGA块的x坐标
    int getY() { return y; }    // 获得FPGA块的y坐标

    vector<RRNode *> &getRRNodes() { return rrNodes; }  // 获得该FPGA块拥有的布线资源节点

    FpgaTile *getDown() const { return down; }  // 获得该FPGA块下方的FPGA块
    void setDown(FpgaTile *down) { this->down = down; } // 设置该FPGA块的下方FPGA块

    FpgaTile *getUp() const { return up; }  // 获得该FPGA块上方的FPGA块
    void setUp(FpgaTile *up) { this->up = up; } // 设置该FPGA块的上方FPGA块

    FpgaTile *getLeft() const { return left; }  // 获得该FPGA块左边的FPGA块
    void setLeft(FpgaTile *left) { this->left = left; } // 设置该FPGA块的左边FPGA块

    FpgaTile *getRight() const { return right; }    // 获得该FPGA块右边的FPGA块
    void setRight(FpgaTile *right) { this->right = right; } // 设置该FPGA块的右边FPGA块

    void generateContents();
    void populateSwitchbox();

    RRNode &getLogicPin(int idx) {  // 获得逻辑块的idx引脚
        assert(logicPin.find(idx) != logicPin.end());
        return *(logicPin[idx]);
    }

    RRNode &getVWire(int idx) {     // 获得FPGA块的垂直导线
        assert((size_t) idx < vWires.size());
        return *(vWires[idx]);
    }

    RRNode &getHWire(int idx) {     // 获得FPGA块的水平导线
        assert((size_t) idx < hWires.size());
        return *(hWires[idx]);
    }

};

#endif