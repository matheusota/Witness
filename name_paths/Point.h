#pragma once
#include <iostream>

struct Point{
    //constructors
    Point(){
        i = 0;
        j = 0;
    }
    
    Point(int new_i, int new_j){
        i = new_i;
        j = new_j;
    }
    
    int i;
    int j;
};

//== operator
inline bool operator==(const Point& p1, const Point& p2){
    return p1.i == p2.i && p1.j == p2.j;
}

