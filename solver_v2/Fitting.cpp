#include "Fitting.h"

//constructor
Fitting::Fitting(int width, int height, char grid[10][10], map <char, int[4][4]> dict){
    this -> dict = dict;
    
    //construct state        
    s = new PieceState(width, height, grid);
}

//destructor
Fitting::~Fitting(){
}

//DFS Algorithm
bool Fitting::Solve(){
    vector<int> actions;
    GetActions(&actions);
    
    //reached goal
    if(CheckGoal())
        return true;
        
    for(int i = 0; i < actions.size(); i++){
        //apply action
        ApplyAction(actions[i]);
        
        //recursive call of DFS to childs
        if(Solve())
            return true;
        
        //backtrack
        else
            UndoAction();
    }

    return false;
}

//get all possible actions, we do this according to De Bruijn algorithm
//an action is the id of the piece to be placed
void Fitting::GetActions(vector<int> *actions){
    Point hole, square;
    
    //find the first hole
    hole = FindHole();
    
    //see pieces that fit it
    for(int i = 0; i < s -> n_pieces; i++){
        
        if(!s -> pieces[i].placed){
            //find the square in the piece that we will try to use to fill the hole
            square = FindSquare(s -> pieces[i].type);
            
            //check if putting this piece will create conflicts, if not, add piece as an action
            if(!CheckConflicts(s -> pieces[i].type, hole, square))
                actions -> push_back(i);
        }
    }
}

//apply an action
void Fitting::ApplyAction(int a){
    int (&piece)[4][4] = dict[s -> pieces[a].type];
    Point hole, square;
    
    hole = FindHole();
    square = FindSquare(s -> pieces[a].type);
    
    //write piece on grid
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            //putting this piece will fill an already filled square
            if(piece[i][j] == 1)
                s -> grid[hole.i - square.i + i][hole.j - square.j + j] = a + 1;
        }
    }
    
    //update rest of the state
    s -> pieces[a].placed = true;
    s -> actions.push_back(a);
}

//undo last action
void Fitting::UndoAction(){
    int a = s -> actions.back();
    
    //remove piece on the grid
    for(int i = 0; i < s -> height; i++){
        for(int j = 0; j < s -> width; j++){
            if(s -> grid[i][j] == a + 1)
                s -> grid[i][j] = 0;
        }
    }
    
    //update rest of the state
    s -> pieces[a].placed = false;
    s -> actions.pop_back();
}

//check if fitted all pieces
bool Fitting::CheckGoal(){
    for(int i = 0; i < s -> n_pieces; i++){
        if(!s -> pieces[i].placed)
            return false;
    }
    
    return true;
}

//find first hole in the grid
Point Fitting::FindHole(){
    Point hole;
    
    for(int i = s -> height - 1; i >= 0; i--){
        for(int j = 0; j < s -> width; j++){
            if(s -> grid[i][j] == 0){
                hole.i = i;
                hole.j = j;
                return hole;
            }
        }
    }
    
    return hole;
}

//find the square with minimum y and x value(in that order)
Point Fitting::FindSquare(char type){
    Point square;
    
    for(int i = 3; i >= 0; i--){
        for(int j = 0; j < 4; j++){
            if((dict[type])[i][j] == 1){
                square.i = i;
                square.j = j;
                return square;
            }
        }
    }
    
    return square;
}

//check for conflicts when arranging a piece
bool Fitting::CheckConflicts(char type, Point hole, Point square){
    int (&piece)[4][4] = dict[type];
    
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(piece[i][j] == 1){
                //check if it is out of bounds
                if((hole.i - square.i + i >= 0 && hole.i - square.i + i < s -> height) && (hole.j - square.j + j >= 0 && hole.j - square.j + j < s -> width)){
                    //putting this piece will fill an already filled square
                    if(s -> grid[hole.i - square.i + i][hole.j - square.j + j] != 0)
                        return true;
                }
                    
                //there is a square out of bounds
                else
                    return true;
            }
        }
    }
    
    return false;
}

//print the grid
void Fitting::PrintGrid(){
    
    cout << "SOLUTION:" << endl;
    
    for(int i = 0; i < s -> height; i++){
        for(int j = 0; j < s -> width; j++){
            if(s -> grid[i][j] == -1)
                cout << "0 ";
            else
                cout << s -> grid[i][j] << " ";
        }
        
        cout << endl;
    }
    
    cout << "PIECES: " << endl;
    
    for(int i = 0; i < s -> n_pieces; i++)
        cout << i + 1 << " -> " << s -> pieces[i].type << endl;
    
}
