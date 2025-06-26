#include <algorithm>
#include "Object.h"

Instance::Instance(int i_x, int i_y, int i_inst_id, bool i_fixed){
    this->x = i_x;
    this->y = i_y;
    this->fixed = i_fixed;
    this->Inst_id = i_inst_id;
}

int Net::evalHPWL(){
    int result = 0;
    int l_min_x = MAX_INT, l_min_y = MAX_INT, l_max_x = -MAX_INT, l_max_y = -MAX_INT;
    for (auto lo_inst : this->connect_inst){
        std::pair<int, int> lo_pos = lo_inst->getPosition();
        l_min_x = std::min(l_min_x, lo_pos.first);
        l_min_y = std::min(l_min_y, lo_pos.second);
        l_max_x = std::max(l_max_x, lo_pos.first);
        l_max_y = std::max(l_max_y, lo_pos.second);
    }
    result = l_max_x - l_min_x + l_max_y - l_min_y;
    return result;
}