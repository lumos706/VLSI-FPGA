#ifndef NET_H
#define NET_H

#include <set>

using namespace std;

class RRNode;

class Net{
public:
    Net(RRNode &source, int idx);
    virtual ~Net();

private:
    RRNode &source; // net的起始引脚
    int idx;    // net编号
    set<RRNode *> sinks;    // net的目标引脚数组
    set<RRNode *> usedRRs;  // 布线的路径

public:
    void addSink(RRNode &dest); // 添加目标引脚
    RRNode &getSource() { return source; }  // 获得起始引脚
    std::set<RRNode *> &getSinks() { return sinks; }    // 获得目标引脚数组
    void finalizeRouting(); // 根据布线的路径设置布线资源节点所属的net

    void clearPath() { usedRRs.clear(); }   // 清空路径
    void addRRToPath(RRNode &node) { usedRRs.insert(&node); }   // 给布线路径添加node
    std::set<RRNode *> &getPath() { return usedRRs; }   // 获得布线路径
    int getIdx() { return idx; }    // 获得net的编号
    bool verifyRouting();   // 检查布线是否成功
};

#endif