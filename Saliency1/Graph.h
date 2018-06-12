
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <cassert>
#include "Color.h"


const int MAX_WIDTH = 500;
const int MAX_HEIGHT = 500;


class Graph {
private:
    int width;
    int height;
    Color_RGB img[MAX_WIDTH * MAX_HEIGHT];
    
public:
    Graph(int w, int h) {
        width = w;
        height = h;
    }
    Graph(std::istream& is) {
        std::string format = "";
        is >> format;
        assert(format == "P3");
        
    
        
        int w, h;
        is >> w >> h;
        width = w;
        height = h;
        
        int intensity;
        is >> intensity;
        assert(intensity == 255);
        
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                Color_RGB color;
                is >> color;
                color.speed_up();
                img[row * width + col] = color;
            }
        }
    }
    
    void print(std::ostream& os) const {
        os << "P3\n" << width << " " << height << "\n255\n";
        
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                os << img[row * width + col];
            }
            os << std::endl;
        }
        
    }
    
    int get_width() {
        return width;
    }
    int get_height() {
        return height;
    }
    
    Color_RGB get_color(int row, int col) {
        return img[row * width + col];
    }
    
    void set_color(int row, int col, Color_RGB color) {
        img[row * width + col] = color;
    }
};

#endif /* Graph_h */
