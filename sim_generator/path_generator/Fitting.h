#include <iostream>
#include <vector>
#include "PieceState.h"
#include <map>

using namespace std;

class Fitting {
public:
    int reached;
    int n_sol;
    
    Fitting(int width, int height, map <char, int[4][4]> *dict);
    virtual ~Fitting();
    
    void SetGrid(char grid[10][10]);
    bool Solve1();
    bool Solve2();
	void GetActions(vector<int> *actions);
	void ApplyAction(int a);
	void UndoAction();
    bool CheckGoal();
    Point FindHole();
    Point FindSquare(char type);
    bool CheckConflicts(char type, Point hole, Point square);
    void CheckReach();
    void PrintGrid();
    
    protected:
    PieceState *s;
    map <char, int[4][4]> *dict;
};

