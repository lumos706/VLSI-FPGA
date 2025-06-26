#ifndef RRNODE_H
#define RRNODE_H

#include <ostream>
#include <vector>

using namespace std;

class RRNode;
class Net;

class RRNode{
public:
    enum rrType { H_WIRE, V_WIRE, CB_WIRE };
    const char *rrTypeStr[3] = {"H_WIRE", "V_WIRE", "CB_WIRE"}; // 分别对应水平导线，垂直导线，逻辑块引脚

    RRNode(rrType type, int x, int y, int idx);
    virtual ~RRNode();

private:
    vector<RRNode *> connections;   // 该布线资源节点与其他布线资源节点的连接关系
    rrType type;    // 布线资源节点类型
    int x;      // 布线资源节点所属的FPGA块的x坐标
    int y;      // 布线资源节点所属的FPGA块的y坐标
    int idx;    // 布线资源节点编号
    Net *net;   // 布线资源节点所属的net

public:
    void connect(RRNode &node);         // 将该布线资源节点与node连接
    bool isConnected(RRNode &node);     // 检查该布线资源节点是否与node连接
    rrType getType() { return type; }   // 获得布线资源节点类型
    int getX() { return x; }            // 获得布线资源节点的x坐标
    int getY() { return y; }            // 获得布线资源节点的y坐标
    int getIdx() { return idx; }        // 获得布线资源节点的编号

    void clearNet() { this->net = nullptr; } // 清除布线资源节点所属的net
    
    bool isUsed() { return net != nullptr; }    // 检查布线资源节点是否已使用
    void setNet(Net &net);              // 设置该布线资源节点所属的net
    Net *getNet() { return net; }       // 获得该布线资源节点所属的net
    std::vector<RRNode *> &getConnections() { return connections; } // 获得该布线资源节点与其他布线资源节点的连接关系

    friend std::ostream &operator<<(std::ostream &out, RRNode const &node) {
        out << "RRNode (" << node.x << ", " << node.y << ")."
            << node.rrTypeStr[node.type] << "." << node.idx;
        return out;
    }
};

#endif