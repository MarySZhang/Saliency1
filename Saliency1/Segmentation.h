//
//  Segmentation.h
//  Saliency1
//
//  modified by maryzhang on 28/05/2018.
//  Copyright (C) 2006 Pedro Felzenszwalb
//

#ifndef SEGMENTATION_H
#define SEGMENTATION_H
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include "Graph.h"
#include "Color.h"
#include "HC.h"



struct uni_elt {
    int rank;
    int p;
    int size;
};

class universe {
public:
    universe(int elements) {
        elts = new uni_elt[elements];
        num = elements;
        for (int i = 0; i < elements; i++) {
            elts[i].rank = 0;
            elts[i].size = 1;
            elts[i].p = i;
        }
    }
    ~universe() {
        delete[] elts;
    }
    int find(int x) {
        int y = x;
        while (y != elts[y].p) {
            y = elts[y].p;
        }
        elts[x].p = y;
        return y;
    }
    
    void join(int x, int y) {
        if (elts[x].rank > elts[y].rank) {
            elts[y].p = x;
            elts[x].size += elts[y].size;
        } else {
            elts[x].p = y;
            elts[y].size += elts[x].size;
            if (elts[x].rank == elts[y].rank) {
                elts[y].rank++;
            }
        }
        num--;
    }
    int size(int x) const { return elts[x].size; }
    int nu_sets() const { return num; }
    
private:
    uni_elt *elts;
    int num;
};


float THRESHOLD(int size, float c) {
    return c / size;
}

struct edge {
    float weight;
    int a, b;
};

bool operator<(const edge &a, const edge &b) {
    return a.weight < b.weight;
}

universe *segment_graph(int num_vertices, int num_edges, edge *edges, float c) {
    // sort edges by weight
    std::sort(edges, edges + num_edges);
    
    // make a disjoint-set forest
    universe *u = new universe(num_vertices);
    
    // init thresholds
    float *threshold = new float[num_vertices];
    for (int i = 0; i < num_vertices; i++)
        threshold[i] = THRESHOLD(1,c);
    
    // for each edge, in non-decreasing weight order...
    for (int i = 0; i < num_edges; i++) {
        edge *edge_ptr = &edges[i];
        
        // components conected by this edge
        int a = u->find(edge_ptr->a);
        int b = u->find(edge_ptr->b);
        if (a != b) {
            if ((edge_ptr->weight <= threshold[a]) &&
                (edge_ptr->weight <= threshold[b])) {
                u->join(a, b);
                a = u->find(a);
                threshold[a] = edge_ptr->weight + THRESHOLD(u->size(a), c);
            }
        }
    }
    
    
    delete[] threshold;
    return u;
}


double diff(Graph *g, int x1, int y1, int x2, int y2) {
    Color_Lab c1 = g->get_color(y1, x1).transLab();
    Color_Lab c2 = g->get_color(y2, x2).transLab();
    return disColorLab(c1, c2);
}

std::map<int, std::vector<int>> SegmentImage(Graph &g, double c, int min_size) {
    int width(g.get_width()), height(g.get_height());
    
    //build graph
    edge *edges = new edge[width * height * 4];
    int num = 0;
    {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (x < width - 1) {
                    edges[num].a = y * width + x;
                    edges[num].b = y * width + (x + 1);
                    edges[num].weight = diff(&g, x, y, x + 1, y);
                    num++;
                }
                
                if (y < height - 1) {
                    edges[num].a = y * width + x;
                    edges[num].b = (y + 1) * width + x;
                    edges[num].weight = diff(&g, x, y, x, y + 1);
                    num++;
                }
                
                if ((x < width - 1) && (y < height - 1)) {
                    edges[num].a = y * width + x;
                    edges[num].b = (y + 1) * width + (x + 1);
                    edges[num].weight = diff(&g, x, y, x + 1, y + 1);
                    num++;
                }
                
                if ((x < width - 1) && (y > 0)) {
                    edges[num].a = y * width + x;
                    edges[num].b = (y - 1) * width + (x + 1);
                    edges[num].weight = diff(&g, x, y, x + 1, y - 1);
                    num++;
                }
            }
        }
    }
    
    // segment
    universe *u = segment_graph(width*height, num, edges, (float)c);

    // post process small components
    for (int i = 0; i < num; i++) {
        int a = u->find(edges[i].a);
        int b = u->find(edges[i].b);
        if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))
            u->join(a, b);
    }
    delete [] edges;
    
    std::map<int, int> marker;
    std::map<int, std::vector<int>> regionIdx;
    int regionNum = 0;
    Graph copy(width, height);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            copy.set_color(i, j, g.get_color(i, j));
        }
    }
    
    
    
    for (int y = 0; y < height; y++) {
//        int *imgIdx = pImgInd.ptr<int>(y);
        for (int x = 0; x < width; x++) {
            int comp = u->find(y * width + x);
            if (marker.find(comp) == marker.end()) {
                marker[comp] = regionNum++;
            }
            
            regionIdx[marker[comp]].push_back(y * width + x);
            
            Color_RGB c = g.get_color(y, x);
            c.set_comp(marker[comp]);
            g.set_color(y, x, c);
        }
    }
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            Color_RGB c;
            int comp = u->find(row * width + col);
            int colorIndex = int((double(marker[comp]) / marker.size()) * 255);
            if (marker[comp] % 3 == 0) {
                c.setRed(colorIndex);
                c.setGreen(170);
                c.setBlue(170);
            }
            if (marker[comp] % 3 == 1) {
                c.setGreen(colorIndex);
                c.setRed(170);
                c.setBlue(170);
            }
            if (marker[comp] % 3 == 2) {
                c.setBlue(colorIndex);
                c.setGreen(170);
                c.setRed(170);
            }
            copy.set_color(row, col, c);
        }
    }
    delete u;
    
    std::cout << "reg: " << regionIdx.size() << std::endl;
    
    return regionIdx;
}

//

#endif /* Segmentation_h */
