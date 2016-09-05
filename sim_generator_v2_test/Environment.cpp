#include "Environment.h"

//constructor
//if alg is 1 it is creating the tree
//if alg is 0 it is traversing it
Environment::Environment(int width, int height, Point start, Point goal, map <char, PieceGrid> *dictShape, map <char, int> *dictNumber, Tree t, int alg, string *paths){
    this -> dictShape = dictShape;
    this -> dictNumber = dictNumber;
    this -> alg = alg;
    this -> t = t;
    this -> paths = paths;
    
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
void Environment::Solve(){
    //initialize some variables
    n_sol = 0;
    openedSection = false;
    removeAction = 'X';
    s -> current = s -> start;
    
    //call the solver
    Solve_DFS(t);
}

//solve using DFS
bool Environment::Solve_DFS(Tree tr){
    char actions[4];
    int n_actions;
    n_actions = GetActions(actions);
    
    //creating the tree
    if(alg == 1){
        //reached goal
        if(CheckGoal()){
            if(FullCheck()){
                tr -> id = n_sol;
                paths[n_sol] = GetPath(tr);
                n_sol++;
            }
            
            return false;
        }
            
        for(int i = 0; i < n_actions; i++){
            //apply action
            if(ApplyAction(actions[i])){
                //recursive call of DFS to childs
                if(Solve_DFS(InsertChild(tr, actions[i])))
                    return true;//execution wil never come here
                
                //backtrack
                else
                    UndoAction();
            }
        }

        return false;
    }
    
    //traversing the tree
    else{
        //reached goal
        if(CheckGoal()){
            if(FullCheck()){
                solutions[n_sol] = tr -> id;
                sections[n_sol] = GetSectionsPieces();
                n_sol++;
            }
            
            return false;
        }
            
        for(int i = 0; i < n_actions; i++){
            //apply action
            if(ApplyAction(actions[i])){
                //recursive call of DFS to childs
                if(Solve_DFS(GetChild(tr, actions[i])))
                    return true;//execution wil never come here
                
                //backtrack
                else
                    UndoAction();
            }
        }

        return false;
    }
}

//get all possible actions
int Environment::GetActions(char actions[4]){
    int n_actions = 0;
    
    //up
    if(s -> current.i != 0 && s -> grid[s -> current.i - 1][s -> current.j].action == '*'){
        actions[n_actions] = 'u';
        n_actions++;
    }
    //down
    if(s -> current.i != s -> height - 1 && s -> grid[s -> current.i + 1][s -> current.j].action == '*'){
        actions[n_actions] = 'd';
        n_actions++;
    }
    //left
    if(s -> current.j != 0 && s -> grid[s -> current.i][s -> current.j - 1].action == '*'){
        actions[n_actions] = 'l';
        n_actions++;
    }
    //right
    if(s -> current.j != s -> width - 1 && s -> grid[s -> current.i][s -> current.j + 1].action == '*'){
        actions[n_actions] = 'r';
        n_actions++;
    }
    
    //remove action that generates impossibility of reaching the goal
    if(removeAction != 'X'){
        for(int i = 0; i < n_actions; i++){
            if(actions[i] == removeAction){
                //shift
                for(int j = i; j < n_actions - 1; j++)
                    actions[j] = actions[j+1];
                
                n_actions--;
            }
        }
            
        removeAction = 'X';
    }
    
    return n_actions;
}

//apply an action
bool Environment::ApplyAction(char a){
    Point p = s -> current;
    Point prev = p;
     
    //cout << "TRIED " << a << endl;
        
    //up
    if(a == 'u')
        p.i--;
    //down
    else if(a == 'd')
        p.i++;
    //left
    else if(a == 'l')
        p.j--;
    //right
    else if(a == 'r')
        p.j++;
    
    //the grid holds what action the path should take to reach a cell
    s -> grid[p.i][p.j].action = a;
    
    //update current position
    s -> current = p;
    
    //update path
    s -> PushEdge(prev, p);
    
    //check if action opens a section
    if(!openedSection){
        //first check if it is in a corner, because a corner never opens a section
        if(!(prev.i == 0 && prev.j == 0) && !(prev.i == 0 && prev.j == s -> width - 1) && !(prev.i == s -> height - 1 && prev.j == 0) && !(prev.i == s -> height - 1 && prev.j == s -> width - 1)){  
            //top wall
            if(prev.i == 0 && a == 'd')
                openedSection = true;
                
            //bottom wall
            else if(prev.i == s -> height - 1 && a == 'u')
                openedSection = true;
            
            //left wall
            else if(prev.j == 0 && a == 'r')
                openedSection = true;
            
            //right wall
            else if(prev.j == s -> width - 1 && a == 'l')
                openedSection = true;
        }
    }
    
    //check if action closes a section, and if so, update sections grid
    if(openedSection){
        if(p.i == 0 || p.i == s -> height - 1 || p.j == 0 || p.j == s -> width - 1){
            CreateSection(p, a);
            
            //undo if section dont obeys pieces restriction
            if(!CheckPieces(s -> n_sec - 1)){
                UndoAction();
                return false;
            }
        }
    }
    
    return true;
}


//undo last action
void Environment::UndoAction(){
    Point p = s -> current;
    Point next = p;
    char a = s -> grid[p.i][p.j].action;
    
    //no need to remove this action anymore
    removeAction = 'X';
    
    //up
    if(a == 'u')
        p.i++;
    //down
    else if(a == 'd')
        p.i--;
    //left
    else if(a == 'l')
        p.j++;
    //right
    else if(a == 'r')
        p.j--;
    
    //erase action taken on current cell
    s -> grid[next.i][next.j].action = '*';
    
    //update current position
    s -> current = p;
    
    //update path
    s -> PopEdge();
    
    //check if undoing this will open a section
    
    //first check if it is in a corner, because a corner never opens a section
    if(!(next.i == 0 && next.j == 0) && !(next.i == 0 && next.j == s -> width - 1) && !(next.i == s -> height - 1 && next.j == 0) && !(next.i == s -> height - 1 && next.j == s -> width - 1)){  
        //top wall
        if(next.i == 0 && a == 'u')
            EraseSection(next, a);
                
        //bottom wall
        else if(next.i == s -> height - 1 && a == 'd')
            EraseSection(next, a);
                
        //left wall
        else if(next.j == 0 && a == 'l')
            EraseSection(next, a);
        
        //right wall
        else if(next.j == s -> width - 1 && a == 'r')
            EraseSection(next, a);
    }
    
    //check if a section is opened or not
    if(p.i != 0 && p.i != s -> height - 1 && p.j != 0 && p.j != s -> width - 1)
        openedSection = true;
    else
        openedSection = false;
}

//check if path reached the goal
bool Environment::CheckGoal(){
    if(s -> current == s -> goal)
        return true;
    else
        return false;
}

//print the path
void Environment::PrintPath(){
    char goalAction, a;
    int out_j;
    char output[2 * s -> height][4 * s -> width];
    
    //creates the printing matrix
    for(int i = 0; i < 2 * s -> height - 1; i++){
        out_j = 0;
        
        //printing no squares
        if(i % 2 == 0){
            for(int j = 0; j < s -> width; j++){
                if(s -> grid[i - i / 2][j].action != '*'){
                    if(i - i / 2 == s -> goal.i && j == s -> goal.j){
                        goalAction = s -> grid[i - i / 2][j].action;
                        output[i][out_j++] = 'G';
                    }
                    else
                        output[i][out_j++] = s -> grid[i - i / 2][j].action;
                    
                    output[i][out_j++] = ' ';
                }
                else{
                    output[i][out_j++] = '.';
                    output[i][out_j++] = ' ';
                }
                
                if(j != s -> width - 1){
                    output[i][out_j++] = '-';
                    output[i][out_j++] = ' ';
                }
            }
        }
        
        //printing grid squares
        else{
            output[i][out_j++] = '|';
            output[i][out_j++] = ' ';
            
            for(int j = 0; j < s -> width - 1; j++){
                if((*dictNumber)[s -> grid[(i - 1)/2][j].piece] == 0)
                    output[i][out_j++] = ' ';
                else
                    output[i][out_j++] = s -> grid[(i - 1)/2][j].piece;
                output[i][out_j++] = ' ';
                output[i][out_j++] = '|';
                output[i][out_j++] = ' ';
            }
        }
        
        output[i][out_j++] = '\n';
        output[i][out_j++] = '\0';
    }
        
    //update with the actions edges
    for(int i = 0; i < 2 * s -> height - 1; i++){
        out_j = 0;
        while(output[i][out_j] != '\0'){
            a = output[i][out_j];
            if(a == 'G' || a == 'u' || a == 'd' || a == 'l' || a == 'r'){
                if(a == 'G'){
                    output[i][out_j] = 'G';
                    a = goalAction;
                }
                else{
                    a = output[i][out_j];
                    output[i][out_j] = '#';
                }
                
                //up
                if(a == 'u')
                    output[i + 1][out_j] = '^';
                
                //down
                else if(a == 'd')
                    output[i - 1][out_j] = 'v';
                
                //left
                else if(a == 'l')
                    output[i][out_j + 2] = '<';
                    
                //right
                else if(a == 'r')
                    output[i][out_j - 2] = '>';
            }
            
            else
                a = output[i][out_j];
            out_j++;
        }
    }
    
    //output to user
    cout << endl << "SOLUTION " << n_sol << ":" << endl;
    for(int i = 0; i < 2 * s -> height - 1; i++)
        cout << output[i];
    cout << endl;
    
    n_sol++;
}

//create a section, to decide to which side of the path is the section we analyze the feasibility of reaching the goal
void Environment::CreateSection(Point p, char a){
    int x, sec;
    Point p1;
             
    //closed at top wall
    if(a == 'u'){
        //now check which side we should go by minimizing distance to goal
        //left minimizes
        if(abs(s -> goal.j - (p.j - 1)) < abs(s -> goal.j - (p.j + 1))){
            p1 = Point(p.i, p.j);
            removeAction = 'r';
        }
        
        //right minimizes
        else{
            p1 = Point(p.i, p.j - 1);
            removeAction = 'l';
        }
    }
    
    //closed at bottom wall
    else if(a == 'd'){
        //now check which side we should go by minimizing distance to goal
        //left minimizes
        if(abs(s -> goal.j - (p.j - 1)) < abs(s -> goal.j - (p.j + 1))){
            p1 = Point(p.i - 1, p.j);
            removeAction = 'r';
        }
        
        //right minimizes
        else{
            p1 = Point(p.i - 1, p.j - 1);
            removeAction = 'l';
        }
    }
    
    //closed at left wall
    else if(a == 'l'){
        //now check which side we should go by minimizing distance to goal
        //up minimizes
        if(abs(s -> goal.i - (p.i - 1)) < abs(s -> goal.i - (p.i + 1))){
            p1 = Point(p.i, p.j);
            removeAction = 'd';
        }
        
        //down minimizes
        else{
            p1 = Point(p.i - 1, p.j );
            removeAction = 'u';
        }
    }
    
    //closed at right wall
    else if(a == 'r'){
        //now check which side we should go by minimizing distance to goal
        //up minimizes
        if(abs(s -> goal.i - (p.i - 1)) < abs(s -> goal.i - (p.i + 1))){
            p1 = Point(p.i, p.j - 1);
            removeAction = 'd';
        }
        
        //down minimizes
        else{
            p1 = Point(p.i - 1, p.j - 1);
            removeAction = 'u';
        }
    }
    
    //creating sections
    sec = s -> grid[p1.i][p1.j].section;
    x = PropSection(p1, s -> n_sec);
    
    //update information about sections
    (s -> numberSections[s -> n_sec]) += x;
    (s -> numberSections[sec]) -= x;    
    (s -> n_sec)++;
    openedSection = false;
}

//erase a section that opened while undoing an action
void Environment::EraseSection(Point p, char a){
    int sec1, sec2, x;
    Point p1, p2;
    
    //opened at top wall
    if(a == 'u'){
        p1 = Point(p.i, p.j - 1);
        p2 = Point(p.i, p.j);
    }
    
    //opened at bottom wall
    else if(a == 'd'){
        p1 = Point(p.i - 1, p.j - 1);
        p2 = Point(p.i - 1, p.j);
    }
    
    //opened at left wall
    else if(a == 'l'){
        p1 = Point(p.i - 1, p.j);
        p2 = Point(p.i, p.j);
    }
    
    //opened at right wall
    else if(a == 'r'){
        p1 = Point(p.i - 1, p.j - 1);
        p2 = Point(p.i, p.j - 1);
    }
    
    //compare the section number to each side of the path
    sec1 = s -> grid[p1.i][p1.j].section;
    sec2 = s -> grid[p2.i][p2.j].section;
    
    if(sec1 > sec2){
        x = PropSection(p1, sec2);
        s -> numberSections[sec1] -= x;
        s -> numberSections[sec2] += x;
    }
    else{
        x = PropSection(p2, sec1);
        s -> numberSections[sec2] -= x;
        s -> numberSections[sec1] += x;
    }
        
    //are we always erasing the section with the biggest number?
    (s -> n_sec)--;
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

//check the pieces covered by itself
void Environment::CoveredByItself(){
    int sum_pieces = 0;
    char grid[10][10];
    
    for(int sec = 0; sec < s -> n_sec; sec++){
        f -> reached = 0;
        f -> n_sol = 0;
        
        for(int i = 0; i < s -> height - 1; i++){
            for(int j = 0; j < s -> width - 1; j++){
                if(s -> grid[i][j].section == sec)
                    sum_pieces += (*dictNumber)[s -> grid[i][j].piece];
            }
        }
        
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
            
            //Solve it
            f -> Solve2();
        }
        
        reached += f -> reached;
        f_sol += f -> n_sol;
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

//get the number of sections with pieces inside
int Environment::GetSectionsPieces(){
    int r = 0;
    bool flag;
    
    for(int k = 0; k < s -> n_sec; k++){
        flag = false;
        
        for(int i = 0; i < s -> height - 1 && !flag; i++){
            for(int j = 0; j < s -> width - 1  && !flag; j++){
                 if(s -> grid[i][j].section == k && s -> grid[i][j].piece != '-'){
                    flag = true;
                    r++;
                 }
            }
        }
    }
    
    return r;
}

//check if at least one pair of solutions have different number of sections with pieces
bool Environment::CheckSectionsPieces(){
    for(int i = 0; i < n_sol - 1; i++){
        for(int j = i + 1; j < n_sol; j++){
            if(sections[i] != sections[j])
                return true;
        }
    }
    
    return false;
}

//add edges of a solution path
void Environment::AddSolution(string path){
    Point a = s -> start;
    Point b;
    
    s -> n_path = 0;
    for(string::iterator it = path.begin(); it < path.end(); it++){
        if(*it == 'u')
            b = Point(a.i - 1, a.j);
            
        else if(*it == 'd')
            b = Point(a.i + 1, a.j);
        
        else if(*it == 'l')
            b = Point(a.i, a.j - 1);
        
        else
            b = Point(a.i, a.j + 1);
        
        s -> PushEdge(a, b);
        a = b;
    }
}

//get the number of section with pieces for 3 boards
int Environment::GetSectionPiecesBoards(char board1[10][10], char board2[10][10], string path){
    int sec = 0;
    int result = 0;
    bool flag;
    
    //add solution edges
    AddSolution(path);
    
    //set sections to -1
    for(int i = 0; i < s -> height - 1; i++){
        for(int j = 0; j < s -> width - 1; j++)
            s -> grid[i][j].section = -1;
    }
    
    //create sections
    for(int i = 0; i < s -> height - 1; i++){
        for(int j = 0; j < s -> width - 1; j++){
            if(s -> grid[i][j].section == -1){
                PropSection(Point(i, j), sec);
                sec++;
            }
        }
    }
    
    //count the number of sections with pieces
    for(int k = 0; k < sec; k++){
        flag = true;
        for(int i = 0; i < s -> height - 1 && flag; i++){
            for(int j = 0; j < s -> width - 1 && flag; j++){
                if(s -> grid[i][j].section == k && (s -> grid[i][j].piece != '-' || board1[i][j] != '-' || board2[i][j] != '-')){
                    result++;
                    flag = false;
                }
            }
        }
    }
    
    return result;
}

