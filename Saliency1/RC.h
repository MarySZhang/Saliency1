//
//  RC.h
//  Saliency1
//
//  Created by maryzhang on 28/05/2018.
//  Copyright © 2018 maryzhang. All rights reserved.
//

#ifndef RC_H
#define RC_H

#include "Segmentation.h"
#include <fstream>
#include <vector>

double disColorRGB(Color_RGB c1, Color_RGB c2) {
    Color_Lab cl1 = c1.transLab();
    Color_Lab cl2 = c2.transLab();
    return disColorLab(cl1, cl2);
}

double disCenter(int idx, int w, int h) {
    int cw = w / 2;
    int ch = h / 2;
    int iw = idx % w;
    int ih = idx / w;
    return sqrt((cw - iw) * (cw - iw) + (ch-ih) * (ch-ih));
}

class RC {
public:
    RC(Graph &g) {
        regIdx = SegmentImage(g, 50, 105);
        
        numReg = (int)regIdx.size();
        std::cout << numReg << std::endl;
        width = g.get_width();
        height = g.get_height();
        std::vector<Color_RGB> *eraseIdx = new std::vector<Color_RGB>;
        for (auto i : regIdx) {
            for (int j = 0; j < i.second.size(); j++) {
                regColFeq[i.first][g.get_color(i.second[j] / width, i.second[j] % width)] += 1;
            }
            for (auto j : regColFeq[i.first]) {
                if (j.second < (int)(0.0001 * i.second.size())) {
                    eraseIdx->push_back(j.first);
                }
            }
            for (auto k : *eraseIdx) {
                regColFeq[i.first].erase(k);
            }
            for (auto j : regColFeq[i.first]) {
                regColFeq[i.first][j.first] /= i.second.size();
            }
        }
        delete eraseIdx;
    }
    
   
    
    void Sal_Reg() {
        
        for (auto k : regIdx) {
            double salVal = 0;
            for (auto i : regIdx) {
                if (i.first != k.first) {
                    double sv = 1;
                    const double sigma_squared = 0 - 0.16;
                    sv *= (double) i.second.size() / (width * height);
                    sv *= dR(k.first, i.first);
                    sv *= exp(dS(k.first, i.first) / sigma_squared);
                    
                    salVal += sv;
                }
            }
            double centerWeight = 0;
            for (int i = 0; i < regIdx[k.first].size(); i++) {
                centerWeight += disCenter(regIdx[k.first][i], width, height) / (2 * sqrt(width * width + height * height));
            }
            centerWeight = (double) centerWeight / regIdx[k.first].size();
            salVal *= exp(-9 * centerWeight * centerWeight);
            regSal[k.first] = salVal;
        }
    }
    
    
    
    double Sal_RC(int idx, Graph &g) {
        //find which region it belongs to
        int row(idx / width);
        int col(idx % width);
        int comp = g.get_color(row, col).get_comp();
        return regSal[comp];
    }
    
    double dS(int comp1, int comp2) {
        int idx1 = findCenter(comp1);
        int idx2 = findCenter(comp2);
        int h1 = idx1 / width;
        int w1 = idx1 % width;
        int h2 = idx2 / width;
        int w2 = idx2 % width;
        double dh = (double) (h2 - h1) / height;
        double dw = (double) (w2 - w1) / width;
        

        return sqrt(dh * dh + dw * dw);
    }
    
    int findCenter(int comp) {
        std::sort(regIdx[comp].begin(), regIdx[comp].end());
        int idx = (int)regIdx[comp].size() / 2;
        return regIdx[comp][idx];
    }
    
    double dR(int compIdx1, int compIdx2) {
        double color_dis = 0;
        for (auto i : regColFeq[compIdx1]) {
            for (auto j : regColFeq[compIdx2]) {
                color_dis += i.second * j.second * disColorLab(i.first.transLab(), j.first.transLab());
            }
        }
        return color_dis;
    }
private:
    int numReg;
    std::map<int, std::vector<int>> regIdx;
    std::map<int, std::map<Color_RGB, double>> regColFeq;
    std::map<int, double> regSal;
    int width, height;
};


#endif /* RC_h */
