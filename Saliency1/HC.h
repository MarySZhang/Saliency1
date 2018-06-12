//
//  HC.h
//  Saliency1
//
//  Created by maryzhang on 22/05/2018.
//  Copyright © 2018 maryzhang. All rights reserved.
//

#ifndef HC_H
#define HC_H

#include <set>
#include "Color.h"
#include "Graph.h"
#include <cmath>
#include <map>
#include <iostream>
#include <vector>

const int FREQUENCY_SENSITIVITY = 80;

double disColorLab(Color_Lab c1, Color_Lab c2) {
    double l1(c1.get_L()), l2(c2.get_L()), a1(c1.get_a());
    double a2(c2.get_a()), b1(c1.get_b()), b2(c2.get_b());
    double D = (l1 - l2) * (l1 - l2);
    D += (a1 - a2) * (a1 - a2);
    D += (b1 - b2) * (b1 - b2);

    return sqrt(D);
}



class HC {
public:
    HC(Graph *g) {
        int numPixel = 0;
        for (int row = 0; row < g->get_height(); row++) {
            for (int col = 0; col < g->get_width(); col++) {
                color_frequencies[g->get_color(row, col).transLab()] += 1;
//                rgb_frequencies[g->get_color(row, col)] += 1;
                numPixel++;
            }
        }
        std::cout << color_frequencies.size() << std::endl;
        double uncounted = 0;
        std::vector<Color_Lab> *eraseIdx = new std::vector<Color_Lab>;
        for (auto i : color_frequencies) {
            if (color_frequencies[i.first] < FREQUENCY_SENSITIVITY) {
                uncounted += i.second;
                eraseIdx->push_back(i.first);
            }
        }
        for (auto i : *eraseIdx) {
            color_frequencies.erase(i);
        }
        
        std::cout << color_frequencies.size() << " " << uncounted << std::endl;

        for (auto i : color_frequencies) {
            color_frequencies[i.first] /= numPixel;
        }
        
        calculate_HC_smooth();


    }
    
    double Sal_HC(Color_Lab c) {
        double saliency = 0;
        for (auto i : color_frequencies) {
            saliency += i.second * disColorLab(c, i.first);
        }
        return saliency;
    }
    
    void calculate_HC_smooth() {
        for (auto i : color_frequencies) {
            Color_Lab c = i.first;
            int m = (int)(color_frequencies.size() / 4);
            std::map<double, Color_Lab> closest_colors;
            for (auto i : color_frequencies) {
                closest_colors[disColorLab(c, i.first)] = i.first;
            }
            double T = 0;
            std::map<double, Color_Lab>::iterator it = closest_colors.begin();
            for (int i = 0; i < m; i++) {
                T += disColorLab(c, it->second);
                it++;
            }
            double saliency = 0;
            for (int i = 0; i < m; i++) {
                saliency += (T - disColorLab(c, it->second)) * Sal_HC(c);
            }
            saliency /= (m - 1) * T;
            color_saliency[c] = saliency;
        }
    }
    
    double Sal_HC_Smooth(int idx, Graph &g) {
        int row = idx / g.get_width();
        int col = idx % g.get_width();
        
        return color_saliency[g.get_color(row, col).transLab()];
    }
    
private:
    std::map<Color_Lab, double> color_frequencies;
    std::map<Color_Lab, double> color_saliency;
};


#endif /* HC_h */
