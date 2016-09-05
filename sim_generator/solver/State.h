#include <iostream>
#include "Point.h"

using namespace std;

struct Edge{
    Point prev;
    Point next;
};

struct Cell{
    char action;
    char piece;
    int section;
};

class State {
    public:
        int width, height;
        Point current;
        Point goal;
        
        Cell grid[10][10];
        
        int numberSections[10];
        int n_sec;
        
        Edge path[1000];
        int n_path;
        
        State(int width, int height, Point start, Point goal, char pieces[10][10]);
        virtual ~State();
        
        void PushEdge(Point prev, Point next);
        void PopEdge();
        bool SearchEdge(Point prev, Point next);
};


