#include <iostream>
#include <algorithm>
#include "Environment.h"
#include "Timer.h"

//check if pieces are part of the equivalent pieces
bool CheckEquivalentPieces(char *pieces, vector<char> *eq_pieces, int n_pieces){
    vector<char>::iterator it;

    for(int i = 0; i < n_pieces; i++){
        it = find (eq_pieces[i].begin(), eq_pieces[i].end(), pieces[i]);
        
        if(it == eq_pieces[i].end())
            return false;
    }
    
    return true;
}
//convert a number base 10 to base 24. Using this we can get the pieces assignment
void GetPieces(uint64_t x, int n_pieces, char *pieces){
    int temp[n_pieces];
    int i = n_pieces - 1;
    char typePieces[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X'};
    
    //initialize temp to zero
    for(int j = 0; j < n_pieces; j++)
        temp[j] = 0;
        
    //convert to base 24
    while(x != 0){
        temp[i] = x % 24;
        x /= 24;
        i--;
    }
    
    //convert to pieces characters
    for(i = 0; i < n_pieces; i++)
        pieces[i] = typePieces[temp[i]];
}

//Get all the pieces obtained from rotation
void GetRotationPieces(char piece, vector<char> *eq_pieces){
    int ind_rot = -1;
    string rot_sets[] = {"ABCD", "EF", "GHIJ", "KLMN", "OPQR", "ST", "U", "VXYZ"};
    eq_pieces -> clear();
    
    //find the rotation set of the piece
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < rot_sets[i].size(); j++){
            if(rot_sets[i][j] == piece){
                ind_rot = i;
                break;
            }
        }
    }
    
    //add equivalent pieces to the vector
    if(ind_rot != -1){
        for(int j = 0; j < rot_sets[ind_rot].size(); j++)
            eq_pieces -> push_back(rot_sets[ind_rot][j]);
    }
}

bool DFS_visit(Environment *e){
    vector<char> actions;
    e -> GetActions(&actions);
    
    //reached goal
    if(e -> CheckGoal()){
        if(e -> FullCheck())
            e -> PrintPath();
        
        return false;
    }
        
    for(int i = 0; i < actions.size(); i++){
        //apply action
        if(e -> ApplyAction(actions[i])){        
            //recursive call of DFS to childs
            if(DFS_visit(e))
                return true;
            
            //backtrack
            else
                e -> UndoAction();
        }
    }

    return false;
}

int main(){
    int width, height;
    Point start, goal;
    char temp[10][10];
    Timer timer;
    vector <char> eq_pieces[10];
    vector <Point> coords;
    int n_pieces = 0;
    char pieces[10];
    Environment *e;
    
    cout << "Insert Size:" << endl;
    cin >> height;
    cin >> width;
    
    cout << "Insert Start Position:" << endl;
    cin >> start.i;
    cin >> start.j;
    
    cout << "Insert Goal Position:" << endl;
    cin >> goal.i;
    cin >> goal.j;
    
    cout << "Insert Pieces:" << endl;
    for(int i = 0; i < height - 1; i++){
        for(int j = 0; j < width - 1; j++){
            cin >> temp[i][j];
            
            if(temp[i][j] != '-'){
                coords.push_back(Point(i, j));
                
                //lowercase, rotated piece
                if(temp[i][j] > 96)
                    GetRotationPieces(temp[i][j] - 32, &eq_pieces[n_pieces]);
                //uppercase
                else
                    eq_pieces[n_pieces].push_back(temp[i][j]);
                    
                n_pieces++;
            }
        }
    }
    
    e = new Environment(width, height, start, goal, temp);
    
    timer.StartTimer();
    
    //generate all possible ways of putting the pieces
    for(uint64_t y = 0; y < pow(24, n_pieces); y++){
        //get the pieces types
        GetPieces(y, n_pieces, pieces);
        
        //check if obtained pieces are part of the equivalent pieces of the board
        if(CheckEquivalentPieces(pieces, eq_pieces, n_pieces)){
            //assign on board
            for(int i = 0; i < n_pieces; i++)
                temp[coords[i].i][coords[i].j] = pieces[i];
        
            for(int i = 0; i < n_pieces; i++)
                cout << pieces[i] << " ";
            cout << endl;
            
            e -> SetPieces(temp);
            
            DFS_visit(e);
        }
    }
    
    cout << "SOLVED!" << endl;
    cout << "TIME TAKEN TO RUN: " << timer.EndTimer() << endl;
    return 0;
}
