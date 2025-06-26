#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <sstream>

#include "Design.h"
#include "FPGA.h"
#include "FpgaTile.h"
#include "Net.h"
#include "RRNode.h"
#include "Solution.h"
#include <chrono>
using namespace std;

/*
    Usage: ./main.exe <circuit_file> <W>
*/

int main(int argc, char *argv[]) {
    assert(argc == 3);
    bool printNets = false;

    string circuit_file = argv[1];
    int W = stoi(argv[2]);

    ifstream fp(circuit_file);
    string line;

    // Get grid size
    getline(fp, line);
    int gridSize = stoi(line) + 1;
    cout << "Grid size: " << gridSize << "x" << gridSize << endl;

    // Get # tracks
    cout << "Number of tracks: " << W << endl;

    // Initialize FPGA and design objects
    FPGA fpga(gridSize, W);
    Design design;

    // Get nets fom circuit file
    int netIdx = 0;
    while(true) {
        getline(fp, line);
        istringstream iss(line);

        // Read source
        string xStr, yStr, pStr;
        iss >> xStr >> yStr >> pStr;
        int x = stoi(xStr), y = stoi(yStr), p = stoi(pStr);

        if(x < 0)   break;
        if(printNets) {
            std::cout << "(" << x << ", " << y << ")." << p << " to ";
        }
        Net *net = new Net(fpga.getTile(x, y).getLogicPin(p), netIdx++);

        // Read list of sinks
        while(iss >> xStr) {
            iss >> yStr;
            iss >> pStr;
            x = stoi(xStr);
            y = stoi(yStr);
            p = stoi(pStr);
            if (printNets){
                std::cout << "(" << x << ", " << y << ")." << p << " ";
            }
            RRNode &sink = fpga.getTile(x, y).getLogicPin(p);
            net->addSink(sink);
        }
        if(printNets) {
            cout << endl;
        }
        design.addNet(*net);
    }

    cout << "Starting Routing" << endl;

    // Initialize router
    // you need to finish
    auto begin = chrono::high_resolution_clock::now();
    Router *router = new MyRouter();
    router->routeDesign(fpga, design);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - begin);
    bool success = design.verifyRouting();
    if(!success) {
        cout << "Error: Routing not complete" << endl;
    } else {
        cout << "Routing check passed" << endl;
        cout << "Segments used: " << fpga.getNumSegmentsUsed() << endl;
        size_t lastSlash = circuit_file.find_last_of("/\\");
        string baseName = (lastSlash == string::npos) ? circuit_file : circuit_file.substr(lastSlash + 1);
        size_t lastDot = baseName.find_last_of(".");
        if (lastDot != string::npos) {
            baseName = baseName.substr(0, lastDot);
        }
        string outputFile = "output/" + baseName + "_routing.txt";
        ofstream out(outputFile);
        if (!out.is_open()) {
            cerr << "Error opening output file: " << outputFile << endl;
            return 1;
        }
        out << W << endl;  // Write the channel width
        out << fpga.getNumSegmentsUsed() << endl;  // Write the number of routing segments used
        out << "Routing time: " << duration.count() << " ms" << endl;
        out.close();
        cout << "Routing results written to " << outputFile << endl;
    }
    
    // delete router;
    delete router;

    return 0;
}