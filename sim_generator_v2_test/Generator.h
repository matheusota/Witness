#include <iostream>
#include "Environment.h"
#include <thread>
#include <mutex>
#include <fstream>
#include <string>
#include <cstdint>
#include <cmath>
#include <iterator> 
#include "Timer.h"
#include "SharedQueue.h"
#include <algorithm>
#include "PieceGrid.h"
#include <cassert>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#define LIMIT_SOLUTIONS 13
#define NUMBER_PATHS 8600
#define NUMBER_PIECES 24
#define NUMBER_BOARDS 100000

typedef struct{
    uint64_t start;
    uint64_t end;
}ThreadWork;

typedef struct{
    uint64_t id_board;
    uint64_t id_piece;
}Board;

typedef struct{
    int width; 
    int height; 
    int n_pieces; 
    Point start; 
    Point goal; 
    map <char, PieceGrid> *dictShape; 
    map <char, int> *dictNumber; 
    int tid; 
    int n_threads; 
    int n_paths; 
    Tree t; 
    int mode;
    uint64_t start2;
    uint64_t end2;
    string *paths;
}PassingArgument;
    
//shared queue to hold threads works
SharedQueue<ThreadWork> q(1000);
boost::mutex m;
bool **table;
int n_table;
Board *boards;
string best_boards[3];
int score;
int **comp_table;
int sol;
int histogram[NUMBER_PATHS];
int hist1, hist2, hist3, lala;
uint64_t total_boards;
uint64_t solution_boards;
uint64_t sections_boards;
int debug[8];

//Utils Functions
uint64_t nchoosek(int n, int k);
void unrank(uint64_t rank, int *board, int count, int spaces, int total);
void ArrayToMatrix(int x, int width, int *i, int *j);
int MatrixToArray(int i, int j, int width);
void GetPieces(uint64_t x, int n_pieces, char *pieces);
void ReadDict(map <char, PieceGrid> *dictShape, map <char, int> *dictNumber);
void GetBoard(int width, int height, uint64_t id_board, uint64_t id_piece, int n_pieces, char temp[10][10]);
string MatrixToString(int width, int height, char temp[10][10]);
void GetRotationPieces(char piece, vector<char> *eq_pieces);
int GetIndex(uint64_t id_board, uint64_t id_piece);

//Main Functions
void* CreateComparisonTable(void* arg1);
void RotatePieces(int width, int height, string paths[NUMBER_PATHS], int n_paths, char board[3][10][10], uint64_t id_board[3], int r_sol);
void GetPaths(int width, int height, Tree t, map <char, PieceGrid> dictShape, map <char, int> dictNumber, string paths[NUMBER_PATHS], int *n_paths);
void* GeneratePuzzle(void* arg1);
void* FindPuzzles(void* arg1);
