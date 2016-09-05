#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <map>
#include "State.h"
#include <fstream>
#include "Fitting.h"
#include "Tree.h"
#include "PieceGrid.h"
#include <list>
#include <cstring>

#define MAX_SOLUTIONS 1262820
using namespace std;

class Environment {
public:
    char **paths;
    int sections[MAX_SOLUTIONS];
    int solutions[MAX_SOLUTIONS];
    int n_sol;
    Fitting *f;
    int alg;
    int reached;
    int f_sol;
    Tree t;
    
    Environment(int width, int height, Point start, Point goal, map <char, PieceGrid> dictShape, map <char, int> dictNumber, Tree t, int alg, char **paths);
    virtual ~Environment();
    
    void SetPieces(char pieces[10][10]);
    void GetAllPaths();
    void Solve();
    bool Solve_DFS(Tree tr);
	int GetActions(char actions[4]);
	bool ApplyAction(char a);
	void UndoAction();
    bool CheckGoal();
    void PrintPath();
    void CreateSection(Point p, char a);
    void EraseSection(Point p, char a);
    int PropSection(Point p, int sec);
    bool CheckPieces(int sec);
    void CoveredByItself(char board[10][10]);
    bool FullCheck();
    int GetSectionsPieces();
    bool CheckSectionsPieces();
    void AddSolution(string path);
    int PostCheck(char board1[10][10], char board2[10][10], string path);

private:
    char lastAction;
    list<char> actions;
    State *s;
    bool openedSection;
    char removeAction;
    map <char, PieceGrid> dictShape;
    map <char, int> dictNumber;
};

