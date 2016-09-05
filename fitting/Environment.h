#include <iostream>
#include <vector>
#include "State.h"
#include <map>
#include <fstream>

using namespace std;

class Environment {
public:
    Environment(int width, int height, char grid[10][10]);
    virtual ~Environment();
   
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
    State *s;
    map <char, int[4][4]> dict;
};

