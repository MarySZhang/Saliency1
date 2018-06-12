//
//  IterCut.h
//  Saliency1
//
//  Created by maryzhang on 05/06/2018.
//  Copyright © 2018 maryzhang. All rights reserved.
//

#ifndef IterCut_h
#define IterCut_h
#include "RC.h"
#include "HC.h"
#include "HRC.h"
#include "Graph.h"
#include "Color.h"


void SaliencyCut(Graph &g, int iterNum) {
    iterNum++;
    int uncertainty = 0;
    std::map<int, std::vector<int>> regIdx = SegmentImage(g, 50, (iterNum + 5) * 21);
    for (auto i : regIdx) {
        for (int j = 0; j < i.second.size(); j++) {
            int col = i.second[j] % g.get_width();
            int row = i.second[j] / g.get_width();
            Color_RGB c = g.get_color(row, col);
            double salVal = (double)c.getSaliency() / 255;
            
            if (salVal > 0.8) {
                salVal *= 1.1;
            } else if (c.getSaliency() < 0.2) {
                salVal *= 0.5;
            } else {
                salVal = (salVal - 0.123) * 1.3;
                uncertainty++;
            }
            c.setSaliency(salVal);
            g.set_color(row, col, c);
        }
    }
        
    std::cout << "Saliency Cut Iteration " << iterNum << "...\n";
    
    std::string name = "cut_iteration_" + std::to_string(iterNum) + ".ppm";
    
    std::ofstream iter(name.c_str());
    g.print(iter);
    
    if (uncertainty > 10 && iterNum < 5 && regIdx.size() > 5) {
        SaliencyCut(g, iterNum);
        return;
    }
    for (int i = 0; i < g.get_width() * g.get_height(); i++) {
        int row = i / g.get_width();
        int col = i % g.get_width();
        Color_RGB c = g.get_color(row, col);
        double salVal = (double)c.getSaliency() / 255;
        if (salVal < 0.3) {
            c.setSaliency(0);
        } else {
            c.setSaliency(1);
        }
        g.set_color(row, col, c);
    }
    
    
}


#endif /* IterCut_h */
