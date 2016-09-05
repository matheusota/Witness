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
        Point start;
        
        Cell grid[10][10];

        int n_sec;
        
        Edge path[1000];
        int n_path;
        
        State(int width, int height, Point start, Point goal);
        virtual ~State();
        
        void SetPieces(char pieces[10][10]);
        void PushEdge(Point prev, Point next);
        void PopEdge();
        bool SearchEdge(Point prev, Point next);
};


