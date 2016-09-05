#include "PieceState.h"

//constructor
PieceState::PieceState(int width, int height){
    this -> width = width;
    this -> height = height;
}

//destructor
PieceState::~PieceState(){
}

//set the grid
void PieceState::SetGrid(char grid[10][10]){
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

//given a piece id, check if it is reached by itself
bool PieceState::ReachPiece(int x){
    int c;
    if(pieces[0].type == 'A' && pieces[0].type == pieces[1].type && pieces[1].type == pieces[2].type)
        c = 0;
        
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(grid[i][j] == x + 1 && CheckTypePosition(Point(i, j), pieces[x].type))
                return true;
        }
    }
    
    return false;
}

//check if a piece is in a given position and have a given type
bool PieceState::CheckTypePosition(Point p, char type){
    for(int i = 0; i < n_pieces; i++){
        if(pieces[i].position == p && pieces[i].type == type)
            return true;
    }
    
    return false;
}
