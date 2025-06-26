#include "FPGA.h"
#include "FpgaTile.h"
#include "RRNode.h"

FPGA::FPGA(int gridSize, int W): N(gridSize), W(W) {
    // Create all tiles
    for(int x = 0; x < N; x++) {
        for(int y = 0; y < N; y++) {
            FpgaTile *tile = new FpgaTile(x, y, W);
            tiles.push_back(tile);
            tileMap[x][y] = tile;
        }
    }

    for(auto tile : tiles) {
        int x = tile->getX();
        int y = tile->getY();
        if(x != 0)  tileMap[x][y]->setLeft(tileMap[x - 1][y]);
        if(x != (N - 1))    tileMap[x][y]->setRight(tileMap[x + 1][y]);
        if(y != 0)  tileMap[x][y]->setUp(tileMap[x][y - 1]);
        if(y != (N - 1))    tileMap[x][y]->setDown(tileMap[x][y + 1]);
    }

    for(auto tile : tiles) {
        tile->generateContents();
    }

    for(auto tile : tiles) {
        tile->populateSwitchbox();
    }
}

FPGA::~FPGA() {
    for(auto tile : tiles) {
        delete tile;
    }
}

int FPGA::getNumSegmentsUsed() {
    int count = 0;
    for(auto tile : tiles) {
        for(auto rrNode : tile->getRRNodes()) {
            if(rrNode->isUsed()) {
                count++;
            }
        }
    }
    return count;
}