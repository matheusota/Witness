#include "State.h"

//constructor
State::State(int width, int height, Point start, Point goal, char pieces[10][10]){
    this -> width = width;
    this -> height = height;
    this -> current = start;
    this -> start = start;
    this -> goal = goal;
    
    n_sec = 0;
    n_path = 0;
    
    //initialize grid
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            grid[i][j].action = '*';
            grid[i][j].piece = pieces[i][j];
            grid[i][j].section = 0;
        }
    }
    
    //initialize sections informations
    for(int i = 0; i < 10; i++)
        numberSections[i] = 0;
    numberSections[0] = (height - 1) * (width - 1);
    n_sec = 1;
    
    cout << "Insert Number of Broken Edges:\n";
    cin >> n_broken;
    
    cout << "Insert Broken Edges:\n";
    for(int i = 0; i < n_broken; i++)
        cin >> broken[i].prev.i >> broken[i].prev.j >> broken[i].next.i >> broken[i].next.j;
    
    int n;
    Point aux;
    cout << "Insert Number of Broken Vertices: \n";
    cin >> n;
    
    for(int i = 0; i < n; i++){
        cin >> aux.i >> aux.j;
        grid[aux.i][aux.j].action = '@';
    }
    
    grid[start.i][start.j].action = 'S';
}

//destructor
State::~State(){
}

//set the state of the pieces
void State::SetPieces(char pieces[10][10]){
    n_sec = 0;
    n_path = 0;
    
    //initialize grid
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            grid[i][j].action = '*';
            grid[i][j].piece = pieces[i][j];
            grid[i][j].section = 0;
        }
    }
    
    //initialize sections informations
    for(int i = 0; i < 10; i++)
        numberSections[i] = 0;
    numberSections[0] = (height - 1) * (width - 1);
    n_sec = 1;
    
    grid[start.i][start.j].action = 'S';
}

//add edge to path
void State::PushEdge(Point prev, Point next){
        
    path[n_path].prev = prev;
    path[n_path].next = next;
    
    n_path++;
}

//remove last edge of path
void State::PopEdge(){
    if(n_path > 0)
        n_path--;
}

//search for an edge in path
bool State::SearchEdge(Point prev, Point next){
        
    for(int i = 0; i < n_path; i++){
        if((path[i].prev == prev && path[i].next == next) || (path[i].prev == next && path[i].next == prev))
            return true;
    }
    
    return false;
}

//search for a broken edge in path
bool State::SearchBrokenEdge(Point prev, Point next){
        
    for(int i = 0; i < n_broken; i++){
        if((broken[i].prev == prev && broken[i].next == next) || (broken[i].prev == next && broken[i].next == prev))
            return true;
    }
    
    return false;
}
