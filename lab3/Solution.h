#ifndef SOLUTION_H
#define SOLUTION_H

class FPGA;
class Design;

class Router {
public:
    Router(){}
    virtual ~Router(){}
    virtual void routeDesign(FPGA &fpga, Design &design) = 0;
};

class MyRouter:public Router{
public:
    MyRouter(){}
    virtual ~MyRouter(){}
    void routeDesign(FPGA &fpga, Design &design);
    
};
#endif