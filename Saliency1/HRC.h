//
//  HRC.h
//  Saliency1
//
//  Created by maryzhang on 31/05/2018.
//  Copyright © 2018 maryzhang. All rights reserved.
//

#ifndef HRC_H
#define HRC_H

#include "Segmentation.h"
#include "HC.h"
#include "RC.h"
#include <map>

class HRC {
public:
    HRC(Graph &g) {
        regIdx = SegmentImage(g, 50, 105);
        HC *hc = new HC(&g);
        for (auto i : regIdx) {
            double sum = 0;
            for (int j = 0; j < i.second.size(); j++) {
                int row = i.second[j] / g.get_width();
                int col = i.second[j] % g.get_width();
                double c = -1 / (double)sqrt(g.get_width() * g.get_width() + g.get_height() * g.get_height());
                double centerWeight = exp(c * disCenter(i.second[j], g.get_width(), g.get_height()));
                sum += (double)(hc->Sal_HC_Smooth(row * g.get_width() + col, g)) * centerWeight;
            }
            sum /= i.second.size();
            regSal[i.first] = sum;
        }
        double min = 10000;
        double max = 0;
        
        for (auto i : regSal) {
            if (i.second > max) {
                max = i.second;
            }
            if (i.second < min) {
                min = i.second;
            }
        }
        for (auto i : regSal) {
            regSal[i.first] -= min;
            regSal[i.first] /= max - min;
//            if (regSal[i.first] > 0.5) {
//                regSal[i.first] = 1;
//            } else {
//                regSal[i.first] = 0;
//            }
        }
        for (int row = 0; row < g.get_height(); row++) {
            for (int col = 0; col < g.get_width(); col++) {
                Color_RGB color = g.get_color(row, col);
                color.setSaliency(regSal[g.get_color(row, col).get_comp()]);
                
                g.set_color(row, col, color);
            }
        }
        delete hc;
    }
    
private:
    std::map<int, std::vector<int>> regIdx;
    std::map<int, double> regSal;
};


#endif /* HRC_h */
