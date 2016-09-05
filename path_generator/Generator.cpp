#include <iostream>
#include "Environment.h"
#include <thread>
#include <mutex>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <vector>
#include <iterator> 
#include "Timer.h"
#include "SharedQueue.h"

using namespace std;

typedef struct{
    uint64_t start;
    uint64_t end;
}ThreadWork;

//shared queue to hold threads works
SharedQueue<ThreadWork> q(1000);
mutex m;
int mode;

//Read pieces dictionary
void ReadDict(map <char, int[4][4]> *dictShape, map <char, int> *dictNumber){
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
void GeneratePuzzle(int width, int height, int n_pieces, Point start, Point goal, map <char, int[4][4]> dictShape, map <char, int> dictNumber, int n_solutions, vector<char> paths[100], int n_paths, int tid, int n_threads){
    int total = (width - 1) * (height - 1);
    int board[10];
    char temp[10][10];
    char pieces[10];
    Point p[10];
    Environment *e = new Environment(width, height, start, goal, &dictShape, &dictNumber, paths, n_paths);
    string s = "temp/"+std::to_string(tid)+".txt";
    ofstream f(s.c_str());
    ThreadWork work;
    int n;
    vector<int> path_ids;
    
    m.lock();
    cout << "STARTED T" << tid << endl;
    m.unlock();
     
    while(true){
        //remove a work piece
        q.WaitRemove(work);
        
        m.lock();
        cout << "T" << tid << " DOING FROM " << work.start << " TO " << work.end << endl;
        m.unlock();
        
        //check if it is the last work
        if(work.start == work.end){
            m.lock();
            cout << "ENDED T" << tid << endl;
            m.unlock();
            
            f.close();
            delete e;
            
            return;
        }
        
        //for this work piece we generate the possible ways of putting the pieces on the board    
        for (uint64_t k = work.start; k < work.end; k++)
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

                //print puzzles that have n_solutions paths as solutions
                n = e -> CheckPaths(&path_ids);
                if(n > 0 && n >= n_solutions){
                    f << "COMMON SOLUTIONS: " << n << endl;
                    for(auto &i: path_ids)
                        f << i << " ";
                    f << endl;
                    
                    for(int i = 0; i < n_pieces; i++)
                        f << p[i].i << " " << p[i].j << " " << pieces[i] << " ";
                    f << endl;
                }
                path_ids.clear();
            }
        }
    }
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
    
    char c;
    vector<char> paths[1000];
    int n_paths;
    int n_solutions;
    string line;
    char sep[] = " ";
    char *token;
    
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
    
    cout << "Insert Number of Paths:" << endl;
    cin >> n_paths;
    
    cout << "Insert Paths:" << endl;
    for(int i = 0; i < n_paths; i++){
        getline(cin, line);
        
        if(line.size() > 3){//this is just to ignore empty lines
            token = strtok(&line[0], sep); //this is c-style not c++
            
            while(token != NULL){
                sscanf(token, "%c", &c);
                paths[i].push_back(c);
                token = strtok(NULL, sep);
            }
        }
        else
            i--;
    }
    
    cout << "Insert Number of Paths that are Solution:" << endl;
    cin >> n_solutions;
    
    timer.StartTimer();
    
    //read dictionary
    ReadDict(&dictShape, &dictNumber);
    
    //create threads to generate puzzles
    for(int i = 0; i < n_threads; i++)
        t[i] = new thread(GeneratePuzzle, width, height, n_pieces, start, goal, dictShape, dictNumber, n_solutions, paths, n_paths, i, n_threads);

    //put the work in the queue
    uint64_t n_states = nchoosek((width - 1) * (height - 1), n_pieces);
    ThreadWork work;
    
    for(int i = 0; i < n_states; i++){
        work.start = i;
        work.end = i + 1;
        
        //last work
        if(work.end > n_states)
            work.end = n_states;
            
        q.WaitAdd(work);
    }
    
    //add zeroed works for every thread to signal that they should finish execution
    work.start = work.end = 0;
    for(int i = 0; i < n_threads; i++)
        q.WaitAdd(work);
    
    //join all threads    
    for(int i = 0; i < n_threads; i++)
        t[i]->join();

    cout << "TIME TAKEN TO RUN: " << timer.EndTimer() << endl;
    delete [] t;
    
    return 0;
}
