//
//  main.cpp
//  Saliency1
//
//  Created by maryzhang on 22/05/2018.
//  Copyright © 2018 maryzhang. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include "Color.h"
#include "Graph.h"
#include "HC.h"
#include "Segmentation.h"
#include "RC.h"
#include "HRC.h"
#include "IterCut.h"
#include "SalCut.h"

using namespace std;

void HistogramCut(char * fileName, bool cut);
void RegionCut(char * fileName, bool cut);
void RegionalHisCut(char * fileName, bool cut);

int main(int argc, char * argv[]) {
    bool cut = false;
    
    if (argc < 4) {
        cout << "USAGE: CUT/NOCUT HC/RC/HRC filename1 filename2 ...\n";
        return 0;
    }
    
    if (string(argv[1]) == "CUT") {
        cut = true;
    } else if (string(argv[1]) == "NOCUT") {
        cut = false;
    } else {
        cout << "USAGE: CUT/NOCUT HC/RC/HRC filename1 filename2 ...\n";
        return 0;
    }
    
    for (int i = 3; i < argc; i++) {
        cout << "File name: " << (string)argv[i] << endl;
        if (string(argv[2]) == "HC") {
            cout << "Beginning Histogram Contrast...\n";
            HistogramCut(argv[i], cut);
            cout << "Histogram Contrast Finished\n";
        }
        
        else if (string(argv[2]) == "RC") {
            cout << "Beginning Region Contrast...\n";
            RegionCut(argv[i], cut);
            cout << "Region Contrast Finished\n";
        }
        
        else if (string(argv[2]) == "HRC") {
            cout << "Beginning Regional Histogram Contrast...\n";
            RegionalHisCut(argv[i], cut);
            cout << "Regional Histogram Contrast Finished\n";
        }
        
        else {
            cout << "USAGE: CUT/NOCUT HC/RC/HRC filename1 filename2 ...\n";
            return 0;
        }
    }
    return 0;
}


void HistogramCut(char *fileName, bool cut) {
    string inFile = string(fileName) + ".ppm";
    string outFile = string(fileName) + "_try_hc.ppm";
    ifstream ins(inFile.c_str());
    if (!ins.is_open()) {
        cout << "Error opening file \n";
        exit(1);
    }
    
    Graph *g = new Graph(ins);
    
    
    HC *hc = new HC(g);
    
    double SalVal = 0;
    double min = 10000;
    double max = 0;
    
    for (int row = 0; row < g->get_height(); row++) {
        for (int col = 0; col < g->get_width(); col++) {
            SalVal = (double)(hc->Sal_HC_Smooth(row * g->get_width() + col, *g));
            
            if (SalVal > max) {
                max = SalVal;
            }
            if (SalVal < min) {
                min = SalVal;
            }
        }
    }
    for (int row = 0; row < g->get_height(); row++) {
        for (int col = 0; col < g->get_width(); col++) {
            
            SalVal = (double)(hc->Sal_HC_Smooth(row * g->get_width() + col, *g));
            SalVal -= min;
            SalVal /= max - min;
            Color_RGB color = g->get_color(row, col);
            color.setSaliency(SalVal);
            g->set_color(row, col, color);
            
        }
    }
    
    if (cut) SaliencyCut(*g, 0);
    
    for (int i = 0; i < g->get_height(); i++) {
        for (int j = 0; j < g->get_width() ; j++) {
            Color_RGB c = g->get_color(i, j);
            c.printSaliency((double)c.getSaliency() / 255);
            g->set_color(i, j, c);
        }
    }
    
    ofstream newGraph(outFile.c_str());
    g->print(newGraph);
    
    delete g;
    delete hc;
}


void RegionCut(char *fileName, bool cut) {
    string inFile = string(fileName) + ".ppm";
    string outFile = string(fileName) + "_try_rc.ppm";
    ifstream ins(inFile.c_str());
    if (!ins.is_open()) {
        cout << "Error opening file \n";
        exit(1);
    }
    Graph *g = new Graph(ins);
    
    
    RC *rc = new RC(*g);
    
    double SalVal = 0;
    double max = -1;
    double min = 10000;
    
    rc->Sal_Reg();
    
    for (int row = 0; row < g->get_height(); row++) {
        for (int col = 0; col < g->get_width(); col++) {
            SalVal = rc->Sal_RC(row * g->get_width() + col, *g);
            
            if (max < SalVal) {
                max = SalVal;
            }
            if (min > SalVal) {
                min = SalVal;
            }
        }
    }
    for (int row = 0; row < g->get_height(); row++) {
        for (int col = 0; col < g->get_width(); col++) {
            SalVal = rc->Sal_RC(row * g->get_width() + col, *g);
            
            SalVal -= min;
            SalVal /= (max - min);
            
            Color_RGB c = g->get_color(row, col);
            c.setSaliency(SalVal);
            g->set_color(row, col, c);
        }
    }
    
    if (cut) SaliencyCut(*g, 0);

    for (int i = 0; i < g->get_height(); i++) {
        for (int j = 0; j < g->get_width() ; j++) {
            Color_RGB c = g->get_color(i, j);
            c.printSaliency((double)c.getSaliency() / 255);
            g->set_color(i, j, c);
        }
    }
    
    ofstream newGraph(outFile.c_str());
    g->print(newGraph);
    
    delete g;
    delete rc;
}

void RegionalHisCut(char *fileName, bool cut){
    string inFile = string(fileName) + ".ppm";
    string outFile = string(fileName) + "_try_hrc.ppm";
    ifstream ins(inFile.c_str());
    if (!ins.is_open()) {
        cout << "Error opening file \n";
        exit(1);
    }
    Graph *g = new Graph(ins);
    HRC *hrc = new HRC(*g);
    
    if (cut) SaliencyCut(*g, 0);
    
    for (int i = 0; i < g->get_height(); i++) {
        for (int j = 0; j < g->get_width() ; j++) {
            Color_RGB c = g->get_color(i, j);
            c.printSaliency((double)c.getSaliency() / 255);
            g->set_color(i, j, c);
        }
    }

    ofstream ouf(outFile.c_str());
    g->print(ouf);
    delete g;
    delete hrc;
}
