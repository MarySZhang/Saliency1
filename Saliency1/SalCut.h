//
//  SalCut.h
//  Saliency1
//
//  Created by maryzhang on 05/06/2018.
//  Copyright © 2018 maryzhang. All rights reserved.
//

#ifndef SalCut_h
#define SalCut_h

#include "IterCut.h"

const int HC_CODE = 1;
const int RC_CODE = 2;
const int HRC_CODE = 3;

void SalCut(Graph &g, int iterNum, int code) {
    iterNum++;
    int  uncertainty = 0;
    std::map<int, std::vector<int>> regIdx = SegmentImage(g, 50, (iterNum + 5 * 20));
    for (auto i : regIdx) {
        for (int j = 0; j < i.second.size(); j++) {
            int col = i.second[j] % g.get_width();
            int row = i.second[j] / g.get_width();
            Color_RGB c = g.get_color(row, col);
            double salVal = (double)c.getSaliency() / 255;
            
            if (salVal > 0.75) {
                c.setBlue(0);
                c.setGreen(0);
            } else if (salVal < 0.25) {
                c.setRed(0);
                c.setBlue(0);
            } else {
                uncertainty++;
            }
            g.set_color(row, col, c);
        }
    }
    std::cout << "Saliency Cut Iteration " << iterNum << "...\n";
    
    std::string name = "cut_iteration_" + std::to_string(iterNum) + ".ppm";
    
    std::ofstream iter(name.c_str());
    g.print(iter);
    
    if (code == HC_CODE) {
        HC *hc = new HC(&g);
        
        double SalVal = 0;
        double min = 10000;
        double max = 0;
        
        for (int row = 0; row < g.get_height(); row++) {
            for (int col = 0; col < g.get_width(); col++) {
                SalVal = (double)(hc->Sal_HC_Smooth(row * g.get_width() + col, g));
                
                if (SalVal > max) {
                    max = SalVal;
                }
                if (SalVal < min) {
                    min = SalVal;
                }
            }
        }
        for (int row = 0; row < g.get_height(); row++) {
            for (int col = 0; col < g.get_width(); col++) {
                
                SalVal = (double)(hc->Sal_HC_Smooth(row * g.get_width() + col, g));
                SalVal -= min;
                SalVal /= max - min;
                Color_RGB color = g.get_color(row, col);
                color.setSaliency(SalVal);
                g.set_color(row, col, color);
                
            }
        }
        delete hc;
    } else if (code == RC_CODE) {
        RC *rc = new RC(g);
        
        double SalVal = 0;
        double max = -1;
        double min = 10000;
        
        rc->Sal_Reg();
        
        for (int row = 0; row < g.get_height(); row++) {
            for (int col = 0; col < g.get_width(); col++) {
                SalVal = rc->Sal_RC(row * g.get_width() + col, g);
                
                if (max < SalVal) {
                    max = SalVal;
                }
                if (min > SalVal) {
                    min = SalVal;
                }
            }
        }
        for (int row = 0; row < g.get_height(); row++) {
            for (int col = 0; col < g.get_width(); col++) {
                SalVal = rc->Sal_RC(row * g.get_width() + col, g);
                
                SalVal -= min;
                SalVal /= (max - min);
                
                Color_RGB c = g.get_color(row, col);
                c.setSaliency(SalVal);
                g.set_color(row, col, c);
            }
        }
        delete rc;
    } else if (code == HRC_CODE) {
        HRC *hrc = new HRC(g);
        delete hrc;
    }
    
    if (uncertainty > 10 && iterNum < 5) {
        SalCut(g, iterNum, code);
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


#endif /* SalCut_h */
