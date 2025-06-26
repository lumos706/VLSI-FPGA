#ifndef FPGA_H
#define FPGA_H

#include <map>
#include <vector>

using namespace std;

class FpgaTile;

class FPGA{
public:
    FPGA(int gridSize, int W);
    virtual ~FPGA();

private:
    int N;  // FPGA的大小
    int W;  // 布线通道宽度
    map<int, map<int, FpgaTile *>> tileMap; // FPGA块的映射
    vector<FpgaTile *> tiles;   // FPGA块数组

public:
    FpgaTile &getTile(int x, int y) { return *(tileMap[x][y]); }    // 获得坐标为(x,y)的FPGA块
    vector<FpgaTile *> &getTiles() { return tiles; }    // 获得FPGA块数组

    int getN() { return N; }    // 获得FPGA的大小
    int getW() { return W; }    // 获得布线通道宽度
    int getNumSegmentsUsed();   // 获得布线后Routing Segment的数量
};

#endif