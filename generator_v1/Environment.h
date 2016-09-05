#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <map>
#include "State.h"
#include <fstream>
#include "Fitting.h"

using namespace std;

class Environment {
public:
    int n_sol;
    
    Environment(int width, int height, Point start, Point goal, map <char, int[4][4]> *dictShape, map <char, int> *dictNumber);
    virtual ~Environment();
    
    void SetPieces(char pieces[10][10]);
    bool Solve();
    bool Solve_DFS();
	void GetActions(vector<char> *actions);
	bool ApplyAction(char a);
	void UndoAction();
    bool CheckGoal();
    void PrintPath();
    void PrintPieces();
    void CreateSection(Point p, char a);
    void EraseSection(Point p, char a);
    int PropSection(Point p, int sec);
    bool CheckPieces(int sec);
    bool FullCheck();

private:
    State *s;
    Fitting *f;
    bool openedSection;
    char removeAction;
    map <char, int[4][4]> *dictShape;
    map <char, int> *dictNumber;
};


