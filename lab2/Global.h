#pragma once
#ifndef GLOBAL_H
#define GLOBAL_H

#include <map>
#include "Arch.h"
#include "Object.h"

extern std::map<int, Net*> glb_net_map;
extern std::map<int, Instance* > glb_inst_map;
extern FPGA glb_fpga;

#endif