#include <iostream>
#include <iomanip>
#include "Arch.h"

bool Block::addInst(Instance *i_inst)
{
    if (this->contain.size() >= MAX_BLOCK_CAPACITY){
        return false;
    }
    this->contain.push_back(i_inst);
    return true;
}

bool Block::clearInst(){
    this->contain.clear();
    return true;
}

FPGA::~FPGA(){
    for (int i = 0; i < this->size_x; i++){
        for (int j = 0; j < this->size_y; j++){
            delete this->fpga_blocks[i][j];
        }
    }
    this->fpga_blocks.clear();
}

void FPGA::initialize()
{
    if (this->size_x <= 0 || this->size_y <= 0)
    {
        std::cout << "FPGA size invaild as (" << this->size_x << ", " << this->size_y << std::endl;
    }
    for (int i = 0; i < this->size_x; i++){
        std::vector<Block* > row;
        for (int j = 0; j < this->size_y; j++){
            Block* temp = new Block;
            row.push_back(temp);
        }
        this->fpga_blocks.push_back(row);
    }
}

void FPGA::setSize(int i_size_x, int i_size_y){
    this->size_x = i_size_x;
    this->size_y = i_size_y;
}

bool FPGA::addInst(int i_x, int i_y, Instance* i_inst){
    Block* block = this->getBlock(i_x, i_y);
    if (!block->addInst(i_inst)){
        std::printf("[WARNING] block (%d, %d) is full\n", i_x, i_y);
        return false;
    }
    return true;
}

Block* FPGA::getBlock(int i_x, int i_y){
    if (i_x < 0 || i_x >= this->size_x || i_y < 0 || i_y >= this->size_y){
        std::printf("[WARNING] block (%d, %d) is out of FPGA\n", i_x, i_y);
        return nullptr;
    }
    return this->fpga_blocks[i_x][i_y];
}

void FPGA::reportFPGA(){
    std::printf("FPGA size is (%d, %d)\n", this->size_x, this->size_y);
    std::printf("Block usage map: \n");
    std::cout << "    |";
    for (int i = 0; i < this->size_y; i++){
        std::cout << "y" << std::setw(3) << std::left << i;
    }
    std::cout << std::endl << "------";
    for (int i = 0; i < this->size_y; i++){
        std::cout << "----";
    }
    std::cout << std::endl;
    for (int i = 0; i < this->size_x; i++){
        std::cout << "x" << std::setw(3) << std::left << i << "|";
        for (int j = 0; j < this->size_y; j++){
            Block* lo_block = this->getBlock(i, j);
            std::cout << std::setw(4) << std::right << lo_block->getInstsCount();
        }
        std::cout << std::endl;
    }
}