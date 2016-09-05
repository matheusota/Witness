#include <iostream>
#include "Environment.h"
#include "Timer.h"

bool DFS_visit(Environment *e){
    vector<char> actions;
    e -> GetActions(&actions);
    
    //reached goal
    if(e -> CheckGoal()){
        if(e -> FullCheck() && e -> s -> n_sec >= 3)
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
    char pieces[10][10];
    Timer timer;
    
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
        for(int j = 0; j < width - 1; j++)
            cin >> pieces[i][j];
    }
    
    timer.StartTimer();
    Environment *e = new Environment(width, height, start, goal, pieces);
    
    cout << "SOLVING..." << endl;
    DFS_visit(e);
    
    cout << "SOLVED!" << endl;
    cout << "TIME TAKEN TO RUN: " << timer.EndTimer() << endl;
    return 0;
}
