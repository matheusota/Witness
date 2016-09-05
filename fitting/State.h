#include <iostream>
#include <vector>

using namespace std;

struct Point{
    //constructors
    Point(){
        i = -1;
        j = -1;
    }
    
    Point(int new_i, int new_j){
        i = new_i;
        j = new_j;
    }
    
    int i;
    int j;
};

typedef struct{
    char type;
    bool placed;
    bool reached;
    Point position;
}piece_type;

//== operator
bool operator==(const Point& p1, const Point& p2);

class State {
    public:
        int width, height;
        int grid[10][10];
        
        //we use the index as the id of the piece
        piece_type pieces[10];
        int n_pieces;
        
        //a vector to hold the performed actions
        vector<int> actions;
         
        State(int width, int height, char grid[10][10]);
        virtual ~State();
        bool ReachPiece(int x);
};

