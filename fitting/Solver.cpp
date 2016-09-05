#include <iostream>
#include <fstream>
#include <ctime>
#include <bitset>
#include "Environment.h"

bool DFS_visit(Environment *e){
    vector<int> actions;
    e -> GetActions(&actions);
    
    //reached goal
    if(e -> CheckGoal()){
        cout << "**************************************\n";
        e -> CheckReach();
        e -> PrintGrid();
        return false;
    }
        
    for(int i = 0; i < actions.size(); i++){
        //apply action
        e -> ApplyAction(actions[i]);
        
        //recursive call of DFS to childs
        if(DFS_visit(e))
            return true;
        
        //backtrack
        else
            e -> UndoAction();
    }

    return false;
}

int main(){
    int width, height;
    char grid[10][10];
    
    //read user info        
    cout << "Insert Size:" << endl;
    cin >> height;
    cin >> width;
    
    cout << "Insert Pieces:" << endl;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++)
            cin >> grid[i][j];
    }
    
    clock_t begin = clock();
    Environment *e = new Environment(width, height, grid);
    DFS_visit(e);
    
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    
    cout << "TIME TAKEN TO RUN: " << elapsed_secs << endl;
    return 0;
}
