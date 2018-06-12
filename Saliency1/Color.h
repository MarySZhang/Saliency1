//
//  Color.h
//  Saliency1
//
//  Created by maryzhang on 22/05/2018.
//  Copyright © 2018 maryzhang. All rights reserved.
//

#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <cmath>



const double X_N = 0.950456;
const double Y_N = 0.008856;
const double Z_N = 1.088754;
const double DELTA = 0;

class Color_Lab {
private:
    double L, a, b;
    
    double trans_func (double t) const {
        if (t > DELTA) {
            return t * t * t;
        }
        return 3 * DELTA * DELTA * (t - 4 / 29);
    }
public:
    Color_Lab() {
        L = 0;
        a = 0;
        b = 0;
    }
    Color_Lab(double L_in, double a_in, double b_in) {
        L = L_in;
        a = a_in;
        b = b_in;
    }
    
    double get_L() const {
        return L;
    }
    
    double get_a() const {
        return a;
    }
    
    double get_b() const {
        return b;
    }
    
};
bool operator<(const Color_Lab &lhs, const Color_Lab &rhs){
    if (lhs.get_L() - rhs.get_L() > 0.00001) {
        return false;
    }
    if (lhs.get_L() - rhs.get_L() < -0.00001) {
        return true;
    }
    if (lhs.get_a() - rhs.get_a() > 0.00001) {
        return false;
    }
    if (lhs.get_a() - rhs.get_a() < -0.00001) {
        return true;
    }
    if (lhs.get_b() - rhs.get_b() < -0.00001) {
        return true;
    }
    return false;
}




class Color_RGB {
public:
    
    Color_RGB(){
        red = 0;
        green = 0;
        blue = 0;
        saliency = 0;
        comp = -1;
    }
    
    Color_RGB(int redVal, int greenVal, int blueVal) {
        red = checkRange(redVal);
        green = checkRange(greenVal);
        blue = checkRange(blueVal);
        saliency = 0;
        comp = -1;
    }
    
    
    void setRed(int redVal) {
        red = checkRange(redVal);
    }
    
    int getRed() const {
        return red;
    }
    
    void setGreen(int greenVal) {
        green = checkRange(greenVal);
    }
    
    int getGreen() const {
        return green;
    }
    
    void setBlue(int blueVal) {
        blue = checkRange(blueVal);
    }
    
    int getBlue() const {
        return blue;
    }
    
    
    void setSaliency(double SalVal) {
        int newSal = checkRange(int(SalVal * 255));
        
        red = newSal;
        green = newSal;
        blue = newSal;
        
        saliency = newSal;
    }
    
    void printSaliency(double SalVal) {
        int newSal = checkRange(int(SalVal * 255));
        
        red = newSal;
        green = newSal;
        blue = newSal;
        
        saliency = newSal;
    }
    
    int getSaliency() {
        return saliency;
    }
    
    
    void read(std::istream& ins) {
        ins >> red >> green >> blue;
        red = checkRange(red);
        green = checkRange(green);
        blue = checkRange(blue);
    }
    
    void write(std::ostream& outs) {
        outs << red << " " << green << " " << blue << " ";
    }
    
    void speed_up() {
        red = twelve(red);
        green = twelve(green);
        blue = twelve(blue);
    }
    
    Color_Lab transLab() const {
        return Color_Lab(get_L(), get_a(), get_b());
    }
    
    int get_comp() {
        return comp;
    }
    
    void set_comp(int c) {
        comp = c;
    }
    
private:
    int red;
    int green;
    int blue;
    double saliency;
    int comp;
    
    
    
    /**
     * Requires: Nothing.
     * Modifies: Nothing.
     * Effects:  Returns val if val is in range [0,255],
     *           otherwise returns the closest of 0 and 255.
     */
    int checkRange(int val) {
        if (val >= 0 && val <= 255) {
            return val;
        }
        else if (abs(val) >= abs(val - 255)) {
            return 255;
        } else {
            return 0;
        }
    }
    
    int twelve(int x) {
        return int(checkRange(x) / 22) * 22;
    }
   
    double get_L() const {
        if (get_y() > Y_N) {
            return 116 * pow(get_y(), 0.333333) - 16;
        }
        return 903.3 * get_y();
    }

    double get_a() const {
        return 500 * (f_t(get_x()) - f_t(get_y())) + DELTA; 
    }

    double get_b() const {
        return 200 * (f_t(get_y()) - f_t(get_z())) + DELTA;
    }
    
    double get_x() const {
        return (0.412453 * red + 0.357580 * green + 0.180423 * blue) / (X_N * 255);
    }
    double get_y() const {
        return (0.212671 * red + 0.715160 * green + 0.072169 * blue) / 255;
    }
    double get_z() const {
        return (0.019334 * red + 0.119193 * green + 0.950224 * blue) / (X_N * 255);
    }
    
    double f_t(double t) const {
        if (t > Y_N) {
            return pow(t, 0.333333);
        }
        return 7.787 * t + 16 / 116;
    }
};

std::istream& operator >> (std::istream& ins, Color_RGB& color)
{
    color.read(ins);
    return ins;
}

std::ostream& operator << (std::ostream& outs, Color_RGB color)
{
    color.write(outs);
    return outs;
}

bool operator==(Color_RGB lhs, Color_RGB rhs) {
    if (lhs.getRed() != rhs.getRed() || lhs.getGreen() != rhs.getGreen()
        || lhs.getBlue() != rhs.getBlue()) {
        return false;
    }
    return true;
}

bool operator==(Color_Lab lhs, Color_Lab rhs) {
    if (lhs.get_L() != rhs.get_L() || lhs.get_a() != rhs.get_a()
        || lhs.get_b() != rhs.get_b()) {
        return false;
    }
    return true;
}

bool operator<(const Color_RGB &lhs, const Color_RGB &rhs){
    if (lhs.getRed() != rhs.getRed()) {
        return lhs.getRed() < rhs.getRed();
    }
    if (lhs.getGreen() != rhs.getGreen()) {
        return lhs.getGreen() < rhs.getGreen();
    }
    return lhs.getBlue() < rhs.getBlue();
}
#endif /* Color_h */
