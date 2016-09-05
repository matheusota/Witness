#include "Environment.h"

//constructor
Environment::Environment(int width, int height, char grid[10][10]){
    char type;
    string line;
    ifstream f;
    int aux[4][4];
    
    //construct the pieces dictionary
    f.open("pieces.txt", ios::in);
    while(!f.eof()){
        getline(f, line);
        sscanf(line.c_str(), "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &type,
            &aux[0][0], &aux[0][1], &aux[0][2], &aux[0][3], &aux[1][0], &aux[1][1], &aux[1][2], &aux[1][3], &aux[2][0], &aux[2][1], &aux[2][2], &aux[2][3], &aux[3][0], &aux[3][1], &aux[3][2], &aux[3][3]);
        
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++)
                dict[type][i][j] = aux[i][j];
        }
    }
    f.close();
    
    //construct state        
    s = new State(width, height, grid);
}

//destructor
Environment::~Environment(){
}

//get all possible actions, we do this according to De Bruijn algorithm
//an action is the id of the piece to be placed
void Environment::GetActions(vector<int> *actions){
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
void Environment::ApplyAction(int a){
    int (&piece)[4][4] = dict[s -> pieces[a].type];
    Point hole, square;
    
    hole = FindHole();
    square = FindSquare(s -> pieces[a].type);
    
    //write piece on grid
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(piece[i][j] == 1)
                s -> grid[hole.i - square.i + i][hole.j - square.j + j] = a + 1;
        }
    }
    
    //update rest of the state
    s -> pieces[a].placed = true;
    s -> actions.push_back(a);
}

//undo last action
void Environment::UndoAction(){
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
bool Environment::CheckGoal(){
    for(int i = 0; i < s -> n_pieces; i++){
        if(!s -> pieces[i].placed)
            return false;
    }
    
    return true;
}

//find first hole in the grid
Point Environment::FindHole(){
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
Point Environment::FindSquare(char type){
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
bool Environment::CheckConflicts(char type, Point hole, Point square){
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

//check if pieces are "reached" by itself on the grid
void Environment::CheckReach(){
    for(int x = 0; x < s -> n_pieces; x++){
        if(s -> ReachPiece(x))
            s -> pieces[x].reached = true;
        else
            s -> pieces[x].reached = false;
    }
}

//print the grid
void Environment::PrintGrid(){
    
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
    
    for(int i = 0; i < s -> n_pieces; i++){
        cout << i + 1 << " -> " << s -> pieces[i].type;
        
        if(s -> pieces[i].reached)
            cout << "*" << endl;
        else
            cout << endl;
    }
    
}
