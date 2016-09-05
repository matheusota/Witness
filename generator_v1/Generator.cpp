#include <iostream>
#include "Environment.h"
#include <thread>
#include <mutex>
#include <fstream>
#include <string>
#include <cstdint>
#include <cmath>
#include "Timer.h"
#include "SharedQueue.h"

using namespace std;
mutex m;

//Read pieces dictionary
void ReadFile(map <char, int[4][4]> *dictShape, map <char, int> *dictNumber){
    ifstream f;
    int aux[4][4];
    int n;
    string line;
    char type;
    
    //construct the pieces dictionary
    f.open("pieces.txt", ios::in);
    while(!f.eof()){
        getline(f, line);
        sscanf(line.c_str(), "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &type,
            &aux[0][0], &aux[0][1], &aux[0][2], &aux[0][3], &aux[1][0], &aux[1][1], &aux[1][2], &aux[1][3], &aux[2][0], &aux[2][1], &aux[2][2], &aux[2][3], &aux[3][0], &aux[3][1], &aux[3][2], &aux[3][3], &n);
        
        //assign shape dictionary
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++)
                (*dictShape)[type][i][j] = aux[i][j];
        }
        
        //assign number dictionary
        (*dictNumber)[type] = n;
    }
    f.close();
}

//Combination of N choose K
uint64_t nchoosek(int n, int k)
{
    uint64_t val = 1;
    for (int x = 0; x < k; x++)
        val *= (n-x);
    for (int x = 1; x <= k; x++)
        val /= x;
    return val;
}

//Unranking Function
void unrank(uint64_t rank, int *board, int count, int spaces, int total)
{
    if (count == 0)
        return;
    uint64_t res = nchoosek(spaces-1, count-1);
    if (rank >= res)
        unrank(rank-res, board, count, spaces-1, total);
    else {
        board[0] = total-spaces;
        unrank(rank, &board[1], count-1, spaces-1, total);
    }
}

//convert a coordinate from the array representation to the matrix one
void ConvertCoord(int x, int width, int *i, int *j){
    *j = x % width;
    *i = (x - (*j)) / width; 
}

//convert a number base 10 to base 24. Using this we can get the pieces assignment
void GetPieces(int x, int n_pieces, char *pieces){
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

//generate a puzzle
void GeneratePuzzle(int width, int height, int tid, int n_pieces, Point start, Point goal, map <char, int[4][4]> dictShape, map <char, int> dictNumber, int n_threads){
    int total = (width - 1) * (height - 1);
    uint64_t n_states = nchoosek(total, n_pieces);
    uint64_t x = (int)(n_states / n_threads);
    int board[10];
    char temp[10][10];
    char pieces[10];
    Point p[10];
    Environment *e = new Environment(width, height, start, goal, &dictShape, &dictNumber);
    string s = "temp/"+std::to_string(tid)+".txt";
    ofstream f(s.c_str());
    
    m.lock();
    cout << "STARTED T" << tid << endl;
    m.unlock();
    
    //here we generate the possible ways of putting 3 pieces on the board    
    for (uint64_t k = tid; k < n_states; k+=n_threads)
    {
        //create blank pieces board
        for(int i = 0; i < height - 1; i++){
            for(int j = 0; j < width - 1; j++)
                temp[i][j] = '-';
        }
    
        unrank(k, board, n_pieces, total, total);
        
        //get the points in the board coords
        for(int i = 0; i < n_pieces; i++)
            ConvertCoord(board[i], width - 1, &p[i].i, &p[i].j);
        
        //for each possible piece assignment, create the respective board
        for(int y = 0; y < pow(24, n_pieces); y++){
            //get the pieces types
            GetPieces(y, n_pieces, pieces);
            
            //assign on board
            for(int i = 0; i < n_pieces; i++)
                temp[p[i].i][p[i].j] = pieces[i];
            
            //change environment
            e -> SetPieces(temp);

            //try to solve
            e -> Solve();
            
            //print that it is solvable
            if(e -> n_sol > 0 && e -> n_sol == 1){
                for(int i = 0; i < n_pieces; i++)
                    f << p[i].i << " " << p[i].j << " " << pieces[i] << " ";
                f << endl;
            }
        }
    }
    f.close();
    delete e;
    
    m.lock();
    cout << "ENDED T" << tid << endl;
    m.unlock();
}

int main(int argc, char *argv[]){
    int width, height;
    Point start, goal;
    thread **t;
    int n_pieces;
    Timer timer;
    map <char, int[4][4]> dictShape;
    map <char, int> dictNumber;
    int n_threads;
    
    if(argv[1] == NULL){
        cout << "MISSING NUMBER OF THREADS!\n";
        return 1;
    }
    n_threads = stoi(argv[1], NULL, 10);
    t = new thread*[n_threads];
    
    cout << "Insert Size:" << endl;
    cin >> height;
    cin >> width;
    
    cout << "Insert Numbers of Pieces:" << endl;
    cin >> n_pieces;
    
    cout << "Insert Start Position:" << endl;
    cin >> start.i;
    cin >> start.j;
    
    cout << "Insert Goal Position:" << endl;
    cin >> goal.i;
    cin >> goal.j;
    
    timer.StartTimer();
    
    //read dictionary
    ReadFile(&dictShape, &dictNumber);
    //GeneratePuzzle(width, height, 1, n_pieces, start, goal);
    
    //create threads to generate puzzles
    for(int i = 0; i < n_threads; i++){
        t[i] = new thread(GeneratePuzzle, width, height, i, n_pieces, start, goal, dictShape, dictNumber, n_threads);
    }
    
    //join all threads
    for(int i = 0; i < n_threads; i++)
        t[i]->join();
    
    cout << "TIME TAKEN TO RUN: " << timer.EndTimer() << endl;
    delete [] t;
    
    return 0;
}
