#include "State.h"

//== operator
bool operator==(const Point& p1, const Point& p2)
{
    return p1.i == p2.i && p1.j == p2.j;
}

//constructor
State::State(int width, int height, char grid[10][10]){
    this -> width = width;
    this -> height = height;
    
    n_pieces = 0;
    
    //initialize grid
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            //* represents an already filled square
            //- represents an empty square
            //(type) - represents a piece
            if(grid[i][j] != '*')
                this -> grid[i][j] = 0;
            else
                this -> grid[i][j] = -1;
            
            //update pieces data
            if(grid[i][j] != '-' && grid[i][j] != '*'){
                pieces[n_pieces].type = grid[i][j];
                pieces[n_pieces].placed = false;
                pieces[n_pieces].position = Point(i, j);
                n_pieces++;
            }
        }
    }
}

//destructor
State::~State(){
}

//given a piece id, check if it is reached by itself
bool State::ReachPiece(int x){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(grid[i][j] == x + 1 && pieces[x].position == Point(i, j))
                return true;
        }
    }
    
    return false;
}
