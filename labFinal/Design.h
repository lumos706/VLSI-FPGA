#ifndef DESIGN_H
#define DESIGN_H

#include <map>
#include <string>
#include <Vector>

using namespace std;

class Net;
class RRNode;

class Design {
public:
    Design();
    virtual ~Design();

private:
    vector<Net *> nets; // 用于存放net的数组

public:
    void addNet(Net &net);  // 添加net
    int getNumNets() { return nets.size(); }    // 获得net的数量
    Net &getNet(int idx) { return *(nets[idx]); }   // 获得指定序号的net
    vector<Net*> &getNets() { return nets; }    // 获得net数组
    bool verifyRouting();   // 检查是否布线成功
};

#endif