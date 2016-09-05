#include "Environment.h"

//constructor
Environment::Environment(int width, int height, Point start, Point goal, map <char, int[4][4]> *dictShape, map <char, int> *dictNumber, vector<char> paths[1000], int n_paths){
    this -> dictShape = dictShape;
    this -> dictNumber = dictNumber;
    this -> n_paths = n_paths;
    
    //copy paths
    for(int i = 0; i < n_paths; i++)
        this -> paths[i] = paths[i];
         
    //construct state
    s = new State(width, height, start, goal);
    
    //construct fitting environment
    f = new Fitting(width - 1, height - 1, dictShape);
}

//destructor
Environment::~Environment(){
    delete s;
    delete f;
}

//change pieces
void Environment::SetPieces(char pieces[10][10]){
    s -> SetPieces(pieces);
}

//call the solver
int Environment::CheckPaths(vector<int> *path_ids){
    int count = 0;
    Point prev, next;
    
    //for each path
    for(int k = 0; k < n_paths; k++){
        s -> n_path = 0;
        
        //add edges
        prev = s -> start;
        for(auto &c : paths[k]) {
            next = prev;
            
            if(c == 'u')
                next.i--;
            else if(c == 'd')
                next.i++;
            else if(c == 'l')
                next.j--;
            else if(c == 'r')
                next.j++;
            
            s -> PushEdge(prev, next);
            prev = next;
        }
        
        //initialize sections
        for(int i = 0; i < s -> height - 1; i++){
            for(int j = 0; j < s -> width - 1; j++)
                s -> grid[i][j].section = -1;
        }
        
        //create sections
        s -> n_sec = 0;
        for(int i = 0; i < s -> height - 1; i++){
            for(int j = 0; j < s -> width - 1; j++){
                if(s -> grid[i][j].section == -1){
                    PropSection(Point(i, j), s -> n_sec);
                    (s -> n_sec)++;
                }
            }
        }
        
        //check each section
        if(FullCheck()){
            path_ids -> push_back(k);
            count++;
        }
    }
    
    return count;
}

//propagate a section to adjacent cells and count the pieces inside, return the number of sqrs changed
int Environment::PropSection(Point p, int sec){
    int sum = 1;
    s -> grid[p.i][p.j].section = sec;
    
    //propagate up
    if(p.i != 0 && s -> grid[p.i - 1][p.j].section != sec && !s -> SearchEdge(Point(p.i, p.j), Point(p.i, p.j + 1)))
        sum += PropSection(Point(p.i - 1, p.j), sec);
    
    //propagate down
    if(p.i != s -> height - 2 && s -> grid[p.i + 1][p.j].section != sec && !s -> SearchEdge(Point(p.i + 1, p.j), Point(p.i + 1, p.j + 1)))
        sum += PropSection(Point(p.i + 1, p.j), sec);
    
    //propagate left
    if(p.j != 0 && s -> grid[p.i][p.j - 1].section != sec && !s -> SearchEdge(Point(p.i, p.j), Point(p.i + 1, p.j)))
        sum += PropSection(Point(p.i, p.j - 1), sec);
    
    //propagate right
    if(p.j != s -> width - 2 && s -> grid[p.i][p.j + 1].section != sec && !s -> SearchEdge(Point(p.i, p.j + 1), Point(p.i + 1, p.j + 1)))
        sum += PropSection(Point(p.i, p.j + 1), sec);
    
    
    return sum;
}

//check if a section obey the pieces restrictions
bool Environment::CheckPieces(int sec){
    int sum_section = 0, sum_pieces = 0;
    char grid[10][10];
    
    for(int i = 0; i < s -> height - 1; i++){
        for(int j = 0; j < s -> width - 1; j++){
            if(s -> grid[i][j].section == sec){
                sum_section++;
                sum_pieces += (*dictNumber)[s -> grid[i][j].piece];
            }
        }
    }
    
    if(sum_pieces > 0 && sum_pieces != sum_section)
        return false;
    else{
        //no need to try to fit zero pieces
        if(sum_pieces > 0){
            //construct a grid with only the section squares available
            for(int i = 0; i <  s -> height - 1; i++){
                for(int j = 0; j < s -> width - 1; j++){
                    if(s -> grid[i][j].section == sec)
                        grid[i][j] = s -> grid[i][j].piece;
                    else
                        grid[i][j] = '*';
                }
            }
                
            //try to fit the pieces
            f -> SetGrid(grid);
            
            return f -> Solve1();
        }
        else
            return true;
    }
}

//check if all sections obey pieces restrictions
bool Environment::FullCheck(){
    for(int i = 0; i < s -> n_sec; i++){
        if(!CheckPieces(i))
            return false;
    }
    
    return true;
}
