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
    Fitting *f;
    int alg;
    int reached;
    int f_sol;
    
    Environment(int width, int height, Point start, Point goal, map <char, int[4][4]> *dictShape, map <char, int> *dictNumber, vector<char> paths[100], int n_paths);
    virtual ~Environment();
    
    void SetPieces(char pieces[10][10]);
    int CheckPaths(vector<int> *path_ids);
    int PropSection(Point p, int sec);
    bool CheckPieces(int sec);
    bool FullCheck();

private:
    State *s;
    map <char, int[4][4]> *dictShape;
    map <char, int> *dictNumber;
    vector<char> paths[1000];
    int n_paths;
};


