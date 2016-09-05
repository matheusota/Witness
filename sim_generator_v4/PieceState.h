#include <iostream>
#include <vector>
#include "Point.h"

using namespace std;

typedef struct{
    char type;
    bool placed;
    bool reached;
    Point position;
}piece_type;

class PieceState {
    public:
        int width, height;
        int grid[10][10];
        
        //we use the index as the id of the piece
        piece_type pieces[10];
        int n_pieces;
        
        //a vector to hold the performed actions
        vector<int> actions;
         
        PieceState(int width, int height);
        virtual ~PieceState();
        
        void SetGrid(char grid[10][10]);
        bool ReachPiece(int x);
        bool CheckTypePosition(Point p, char type);
};

