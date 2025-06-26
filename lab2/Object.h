#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#define MAX_INT 0x3f3f3f3f

#include <set>
#include <iostream>

class Net;

class Instance
{
private:
    int x;
    int y;
    bool fixed;
    int Inst_id;
    std::set<Net *> connect_net; // 相连的net编号
public:
    Instance()
    {
        x = -1;
        y = -1;
        fixed = false;
        Inst_id = -1;
    }
    Instance(const Instance& other) {
        this->x = other.x;
        this->y = other.y;
        this->fixed = other.fixed;
        this->Inst_id = other.Inst_id;
        this->connect_net = other.connect_net; // 假设 connectedNets 是浅拷贝即可
    }
    Instance(int i_x, int i_y, int i_inst_id, bool fixed = false);

    void setFixed(bool i_fixed) { this->fixed = i_fixed; }
    // 元件是否不可移动，true表示不可移动，false可以移动
    bool isFixed() { return fixed; }
    std::set<Net *> getNets() { return connect_net; }
    // 给定 net 编号，加入到链接 net 的记录集合中
    void addNet(Net *i_net_id) { this->connect_net.insert(i_net_id); };
    // 设置元件的坐标点
    void setPosition(int i_x, int i_y)
    {
        if (this->isFixed()){
            std::cout << "[WARNING] inst " << this->Inst_id << " is fixed, can not move." << std::endl;
            return;
        }
        this->x = i_x;
        this->y = i_y;
    }

    // pair.first 表示 x 坐标，pair.second 表示 y 坐标
    std::pair<int, int> getPosition() { return std::pair<int, int>(this->x, this->y); }
    // 设置 inst 的 id
    void setInstId(int i_inst_id) { this->Inst_id = i_inst_id; }
    // 获得 inst 的 id
    int getInstId() { return this->Inst_id; }
};

class Net
{
private:
    int net_id;
    std::set<Instance *> connect_inst;

public:
    Net() { net_id = -1; }

    // 设置 net id
    void setNetId(int i_net_id) { this->net_id = i_net_id; }
    // 增加连接的 inst 实例
    void addInst(Instance *i_inst) { this->connect_inst.insert(i_inst); }
    // 获得当前 net 连接的所有 inst 实例对象
    std::set<Instance *> getInsts() { return this->connect_inst; }
    // 返回当前 net 的预估线长
    int evalHPWL();
};

#endif