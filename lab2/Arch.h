#pragma once
#ifndef ARCH_H
#define ARCH_H

#include <vector>
#include "Object.h"

#define MAX_BLOCK_CAPACITY 1

class Block
{
private:
    std::vector<Instance *> contain;

public:
    // 需要注意，一个 Block 只能容纳 MAX_BLOCK_CAPACITY 个 inst 对象
    bool addInst(Instance *i_inst);
    // 清理当前 Block 中分配的 inst 对象
    bool clearInst();
    // 获得当前 Block 存放的 inst 对象集合
    std::vector<Instance *> getInsts() { return this->contain; }
    // 获得当前 Block 中存放的 inst 对象数量
    int getInstsCount() { return (int)this->contain.size(); }
};

class FPGA
{
private:
    std::vector<std::vector<Block *>> fpga_blocks;
    int size_x, size_y;

public:
    FPGA()
    {
        this->size_x = 0;
        this->size_y = 0;
    };
    ~FPGA();

    // 初始化芯片
    void initialize();
    // 设定芯片的规模
    void setSize(int i_size_x, int i_size_y);
    // 获得芯片规模
    int getSizeX() { return this->size_x; }
    int getSizeY() { return this->size_y; }
    // 向指定位置添加 inst 对象
    bool addInst(int i_x, int i_y, Instance *i_inst);
    // 清除指定位置的 inst 对象
    void clearInst(int i_x, int i_y) { this->fpga_blocks[i_x][i_y]->clearInst(); }
    // 访问 FPGA 中特定的 Block
    Block *getBlock(int i_x, int i_y);
    // 报告芯片的基本信息
    void reportFPGA();
};

#endif