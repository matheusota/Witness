#include "PieceState.h"

//constructor
PieceState::PieceState(int width, int height, char grid[10][10]){
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
                n_pieces++;
            }
        }
    }
}

//destructor
PieceState::~PieceState(){
}
