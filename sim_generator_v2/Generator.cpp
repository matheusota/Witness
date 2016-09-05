#include "Generator.h"

using namespace std;

//###########################################################################################################################
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
void ArrayToMatrix(int x, int width, int *i, int *j){
    *j = x % width;
    *i = (x - (*j)) / width; 
}

//convert a coordinate from the matrix representation to the array one
int MatrixToArray(int i, int j, int width){
    return width * i + j;
}

//convert a number base 10 to base 24. Using this we can get the pieces assignment
void GetPieces(uint64_t x, int n_pieces, char *pieces){
    int temp[n_pieces];
    int i = n_pieces - 1;
    char typePieces[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X'};
    //char typePieces[] = {'A','B','C', 'D','E','F', 'G'};
    
    //initialize temp to zero
    for(int j = 0; j < n_pieces; j++)
        temp[j] = 0;
        
    //convert to base 24
    while(x != 0){
        temp[i] = x % NUMBER_PIECES;
        x /= NUMBER_PIECES;
        i--;
    }
    
    //convert to pieces characters
    for(i = 0; i < n_pieces; i++)
        pieces[i] = typePieces[temp[i]];
}

//Get the id of the piece, we consider the pieces a number in base 24 and convert it to base 10.
uint64_t GetPieceId(int n_pieces, char *pieces){
    char typePieces[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X'};
    //char typePieces[] = {'A','B','C', 'D','E','F', 'G'};
    uint64_t result = 0;
    int mult = 0;
    int j;
    
    for(int i = n_pieces - 1; i >= 0; i--){
        for(j = 0; j < NUMBER_PIECES; j++){
            if(typePieces[j] == pieces[i])
                break;
        }
        
        result += j * pow(NUMBER_PIECES, mult);
        mult++;
    }
    
    return result;
}

//Read pieces dictionary
void ReadDict(map <char, PieceGrid> *dictShape, map <char, int> *dictNumber){
    ifstream f;
    PieceGrid aux;
    int n;
    string line;
    char type;
    
    //construct the pieces dictionary
    f.open("pieces.txt", ios::in);
    while(!f.eof()){
        getline(f, line);
        sscanf(line.c_str(), "%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &type,
            &aux.grid[0][0], &aux.grid[0][1], &aux.grid[0][2], &aux.grid[0][3], &aux.grid[1][0], &aux.grid[1][1], &aux.grid[1][2], &aux.grid[1][3], 
            &aux.grid[2][0], &aux.grid[2][1], &aux.grid[2][2], &aux.grid[2][3], &aux.grid[3][0], &aux.grid[3][1], &aux.grid[3][2], &aux.grid[3][3], &n);
        
        //assign shape dictionary
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++)
                (*dictShape)[type].grid[i][j] = aux.grid[i][j];
        }
        
        //assign number dictionary
        (*dictNumber)[type] = n;
    }
    f.close();
}

//convert from the board id and piece id to an actual board
void GetBoard(int width, int height, uint64_t id_board, uint64_t id_piece, int n_pieces, char temp[10][10]){
    string s = "";
    int board[10];
    char pieces[10];
    Point p[10];
    int total = (width - 1) * (height - 1);
    
    //create blank pieces board
    for(int i = 0; i < height - 1; i++){
        for(int j = 0; j < width - 1; j++)
            temp[i][j] = '-';
    }
    
    //unrank board id
    unrank(id_board, board, n_pieces, total, total);
            
    //get the points in the board coords
    for(int i = 0; i < n_pieces; i++)
        ArrayToMatrix(board[i], width - 1, &p[i].i, &p[i].j);
    
    //unrank pieces id
    GetPieces(id_piece, n_pieces, pieces);
    
    //assign on board
    for(int i = 0; i < n_pieces; i++)
        temp[p[i].i][p[i].j] = pieces[i];
}

//convert from matrix to string
string MatrixToString(int width, int height, char temp[10][10]){
    string s = "";
    
    //put all on a string                
    for(int i = 0; i < height - 1; i++){
        for(int j = 0; j < width - 1; j++)
            s += temp[i][j];
        s += "\n";
    }
    
    return s;
}

//Get all the pieces obtained from rotation
void GetRotationPieces(char piece, vector<char> *eq_pieces){
    int ind_rot = -1;
    string rot_sets[] = {"BC", "DEFG", "HI", "JKLM", "NOPQ", "RS", "UVWX"};
    eq_pieces -> clear();
    
    //find the rotation set of the piece
    for(int i = 0; i < 7; i++){
        for(int j = 0; j < rot_sets[i].size(); j++){
            if(rot_sets[i][j] == piece){
                ind_rot = i;
                break;
            }
        }
    }
    
    //add equivalent pieces to the vector
    if(ind_rot != -1){
        for(int j = 0; j < rot_sets[ind_rot].size(); j++)
            eq_pieces -> push_back(rot_sets[ind_rot][j]);
    }
}

//Get the index of a board
int GetIndex(uint64_t id_board, uint64_t id_piece){
    for(int i = 0; i < n_table; i++){
        if(boards[i].id_board == id_board && boards[i].id_piece == id_piece)
            return i;
    }
    
    //This board is not at the table
    return -1;
}

//###########################################################################################################################

//Create the comparison table
void CreateComparisonTable(int tid, int n_paths, int n_threads){
    bool x, y;
    int work_size = n_table/n_threads;
    int start = tid * work_size;
    int end;
    
    if(tid == n_threads - 1)
        end = n_table;
    else
        end = (tid + 1) * work_size;
    
    m.lock();
    cout << "STARTED T" << tid << endl;
    cout << "DOING FROM " << start << " TO " << end << endl;
    m.unlock();
    
    //for this work piece we generate the possible ways of putting the pieces on the board    
    for (int i = start; i < end; i++){
        for(int j = 0; j < n_table; j++){
            m.lock();
            comp_table[i][j] = 0;
            m.unlock();
            
            if(i != j){
                for(int k = 0; k < n_paths; k++){
                    x = table[i][k];
                    y = table[j][k];
                    
                    m.lock();
                    if(x && y)
                        comp_table[i][j]++;
                    m.unlock();
                }
            }
        }
    }
}

//Check if changing for rotating pieces changes the number of common solutions for more than 1
void RotatePieces(int width, int height, string paths[NUMBER_PATHS], int n_paths, char board[3][10][10], uint64_t id_board[3], int r_sol){
    vector<char> pieces[3];
    vector<bool> rotated[3];
    vector<bool> merge_path;
    vector<bool> backup_path;
    vector<bool> r_path[3];
    vector<Point> coords[3];
    vector<char> eq_pieces;
    uint64_t id_piece[3];
    int board_index[3];
    char temp[10][10];
    bool available, can_rotate;
    char aux;
    int y = 0;
    
    //get the pieces on the boards
    for(int k = 0; k < 3; k++){
        for(int i = 0; i < height - 1; i++){
            for(int j = 0; j < width - 1; j++){
                if(board[k][i][j] != '-'){
                    pieces[k].push_back(board[k][i][j]);
                    rotated[k].push_back(false);
                    coords[k].push_back(Point(i, j));
                }
            }
        }
    }
    
    merge_path.resize(NUMBER_PATHS);
    backup_path.resize(NUMBER_PATHS);
    for(int x = 0; x < n_paths; x++)
        merge_path[x] = false;
        
    for(int k = 0; k < 3; k++)
        r_path[k].resize(NUMBER_PATHS);
    
    for(int k = 0; k < 3; k++){
        //get the piece ids
        id_piece[k] = GetPieceId(pieces[k].size(), &pieces[k][0]);
                
        //get the board indexes
        board_index[k] = GetIndex(id_board[k], id_piece[k]);
        
        //assign to r_path
        for(int x = 0; x < n_paths; x++)
            r_path[k][x] = table[board_index[k]][x];
    }
    
    //for each board
    for(int i = 0; i < 3; i++){
        //for each piece in the board
        y = 0;
        for(int j = 0; j < pieces[i].size(); j++){
            //get all the equivalent pieces
            GetRotationPieces(pieces[i][j], &eq_pieces);
                
            available = true;
            aux = pieces[i][j];
            
            //for each eq. piece
            for(int k = 0; k < eq_pieces.size(); k++){
                //change to an eq. piece
                pieces[i][j] = eq_pieces[k];
                
                //get the piece ids
                for(int x = 0; x < 3; x++)
                    id_piece[x] = GetPieceId(pieces[x].size(), &pieces[x][0]);
                
                //get the board indexes
                for(int x = 0; x < 3; x++)
                    board_index[x] = GetIndex(id_board[x], id_piece[x]);
                
                //add to merge_path
                if(board_index[0] > 0 && board_index[1] > 0 && board_index[2] > 0){
                    for(int x = 0; x < n_paths; x++)
                        merge_path[x] = merge_path[x] || table[board_index[i]][x];
                }
                
                //board is not available
                else{
                    available = false;
                    break;
                }
            }
            
            if(available && eq_pieces.size() > 0){
                //create backup
                for(int x = 0; x < n_paths; x++)
                    backup_path[x] = r_path[i][x];
                        
                //first piece that can be rotated, just assign to r_path
                if(y == 0){
                    for(int x = 0; x < n_paths; x++)
                        r_path[i][x] = merge_path[x];
                }
            
                //intersect with r_path
                else{
                    for(int x = 0; x < n_paths; x++)
                        r_path[i][x] = r_path[i][x] && merge_path[x];
                }
            
                //compare r_paths
                can_rotate = true;
                for(int x = 0; x < n_paths; x++){
                    if(r_path[0][x] && r_path[1][x] && r_path[2][x] && x != r_sol){
                        can_rotate = false;
                        break;
                    }
                }
                
                //this piece can be rotated!
                if(can_rotate){
                    y++;
                    rotated[i][j] = true;
                }
                
                //cant rotate, reset r_path
                else{
                    for(int x = 0; x < n_paths; x++)
                        r_path[i][x] = backup_path[x];
                }
            }
            
            //put original piece back
            pieces[i][j] = aux;
        }
    }
    
    //construct final boards
    for(int k = 0; k < 3; k++){
        //create a board with no pieces
        for(int i = 0; i < height - 1; i++){
            for(int j = 0; j < width - 1; j++)
                temp[i][j] = '-';
        }
        
        for(int i = 0; i < coords[k].size(); i++){
            //lower case to represent rotation
            if(rotated[k][i])
                pieces[k][i] += 32;
        }
        
        for(int i = 0; i < coords[k].size(); i++)
            temp[coords[k][i].i][coords[k][i].j] = pieces[k][i];
        
        //assign to best boards
        best_boards[k] = MatrixToString(width, height, temp);
    }
    
    
    /*cout << "score: " << score << "\n";    
    for(int i = 0; i < 3; i++)
        cout << best_boards[i] << endl; 
    cout << "Solution: " << paths[r_sol] << endl;*/
}

//get all the possible paths and put on a tree
void GetPaths(int width, int height, Tree t, map <char, PieceGrid> dictShape, map <char, int> dictNumber, string paths[NUMBER_PATHS], int *n_paths){
    Environment *e = new Environment(width, height, Point(height - 1, 0), Point(0, width - 1), &dictShape, &dictNumber, t, 1, paths);
    char board[10][10];
    
    //create a board with no pieces
    for(int i = 0; i < height - 1; i++){
        for(int j = 0; j < width - 1; j++)
            board[i][j] = '-';
    }
    
    e -> SetPieces(board);
    e -> Solve();
    
    *n_paths = e -> n_sol;
    
    delete e;
}
 
//generate a puzzle or create histogram
void GeneratePuzzle(int width, int height, int n_pieces, Point start, Point goal, map <char, PieceGrid> dictShape, map <char, int> dictNumber, int tid, int n_threads, int n_paths, Tree t, int mode){
    int total = (width - 1) * (height - 1);
    int board[10];
    char temp[10][10];
    char pieces[10];
    Point p[10];
    Environment *e = new Environment(width, height, start, goal, &dictShape, &dictNumber, t, 0, NULL);
    ThreadWork work;
    //string solutions[1000];
    //int n_solutions;
    bool founded;
    
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
                ArrayToMatrix(board[i], width - 1, &p[i].i, &p[i].j);
            
            //for each possible piece assignment, create the respective board
            for(uint64_t y = 0; y < pow(NUMBER_PIECES, n_pieces); y++){
                m.lock();
                total_boards++;
                m.unlock();
                
                //get the pieces types
                GetPieces(y, n_pieces, pieces);
                
                //assign on board
                for(int i = 0; i < n_pieces; i++)
                    temp[p[i].i][p[i].j] = pieces[i];
                
                //change environment
                e -> SetPieces(temp);

                //try to solve
                e -> Solve();
                
                //add solutions to the table
                m.lock();
                if(e -> n_sol > 0){
                    solution_boards++;
                    if(e -> CheckSectionsPieces()){
                        sections_boards++;
                        if(mode == 0){
                            if(e -> n_sol >= LIMIT_SOLUTIONS){
                                boards[n_table].id_board = k;
                                boards[n_table].id_piece = y;
                                
                                for(int i = 0; i < n_paths; i++){
                                    founded = false;
                                    
                                    for(int j = 0; j < e -> n_sol; j++){
                                        if(i == e -> solutions[j]){
                                            founded = true;
                                            break;
                                        }
                                    }
                                    
                                    table[n_table][i] = founded;
                                }
                                
                                n_table++;
                            }
                        }
                        /*else{
                            histogram[e -> n_sol]++;
                            if(e -> n_sol > max_hist)
                                max_hist = e -> n_sol;
                        }*/
                    }
                }
                m.unlock();
            }
        }
    }
}

//try to find the puzzle with the maximum score
void FindPuzzles(int width, int height, int tid, string paths[NUMBER_PATHS], int n_paths){
    ThreadWork work;
    unsigned int aux;
    int board[10];
    int a, b, c;
    int ab, bc, ac, abc, abc_sol;
    char temp[3][10][10];
    uint64_t ids[3];
    Environment *e = new Environment(width, height, Point(width - 1, 0), Point(0, width - 1), NULL, NULL, NULL, 0, NULL);
    
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
            
            delete e;
            
            return;
        }
        
        //for this work piece we check the score of the three boards            
        for (uint64_t y = work.start; y < work.end; y++)
        {
            unrank(y, board, 3, n_table, n_table);
            a = board[0];
            b = board[1];
            c = board[2];
            
            ab = comp_table[a][b];
            /*//jump until a and b changes
            if(ab < score)
                y += n_table - c;*/
            
            if(a != b && b != c && a != c){
                bc = comp_table[b][c];
                ac = comp_table[a][c];
                aux = min(min(ab, bc), ac);
                
                if(aux > 0 /*score < aux*/){
                    //get the number of common solutions between the 3 puzzles
                    abc = 0;
                    for(int x = 0; x < n_paths; x++){
                        if(table[a][x] && table[b][x] && table[c][x]){
                            abc++;
                            abc_sol = x;
                        }
                    
                        if(abc > 1)
                            break;
                    }
                    
                    if(abc == 1){
                        //get the boards
                        GetBoard(width, height, boards[a].id_board, boards[a].id_piece, 3, temp[0]);
                        GetBoard(width, height, boards[b].id_board, boards[b].id_piece, 3, temp[1]); 
                        GetBoard(width, height, boards[c].id_board, boards[c].id_piece, 3, temp[2]);
                        
                        //set first board on environment
                        e -> SetPieces(temp[0]);
                        
                        //check if the boards have at least 2 sections with pieces inside
                        if(/*score < aux &&*/ e -> GetSectionPiecesBoards(temp[1], temp[2], paths[abc_sol]) >= 2){ 
                            m.lock();
                            if(aux == 1)
                                hist1++;
                            else if(aux == 2)
                                hist2++;
                            else if(aux == 3)
                                hist3++;
                            assert(aux < NUMBER_PATHS);
                            //update solution and score
                            //sol = abc_sol;
                            histogram[aux]++;
                            //score = aux;
                            
                            //get ids of the boards
                            //ids[0] = boards[a].id_board;
                            //ids[1] = boards[b].id_board;
                            //ids[2] = boards[c].id_board;
                            
                            //rotate and assign 
                            //RotatePieces(width, height, paths, n_paths, temp, ids, sol);
                            m.unlock();
                        }
                    }
                }
            }
        }
    }
}

int main(){
    string paths[NUMBER_PATHS];
    int n_paths;
    int size;
    int n_threads = thread::hardware_concurrency();
    thread **t;
    Timer timer;
    map <char, PieceGrid> dictShape;
    map <char, int> dictNumber;
    Tree tr = (Tree)malloc(sizeof(struct node));
    tr -> id = -1;
    tr -> u = tr -> d = tr -> r = tr -> l = tr -> parent = NULL;
    tr -> a = 'X';
    t = new thread*[n_threads];
    
    uint64_t n_states;
    ThreadWork work;
    uint64_t work_size;
    
    boards = (Board*) malloc(sizeof(Board) * NUMBER_BOARDS);
    table = (bool**) malloc(sizeof(bool*) * NUMBER_BOARDS);
    comp_table = (int**) malloc(sizeof(int*) * NUMBER_BOARDS);
    for(int i = 0; i < NUMBER_BOARDS; i++){
        comp_table[i] = (int*) malloc(sizeof(int) * NUMBER_PATHS);
        table[i] = (bool*) malloc(sizeof(bool) * NUMBER_PATHS);
    }
    n_table = 0;
    
    total_boards = solution_boards = sections_boards = 0;
    
    //get puzzle size
    cout << "Insert Size:\n";
    cin >> size;
    
    timer.StartTimer();
    
    //read dictionary
    ReadDict(&dictShape, &dictNumber);
    
    //get and read all possible paths in this board 
    cout << "Constructing all paths...\n";
    GetPaths(size, size, tr, dictShape, dictNumber, paths, &n_paths);
    fill(histogram, histogram + NUMBER_PATHS, 0);
    hist1 = hist2 = hist3 = 0;
    
    /*cout << "Creating histogram...\n";
    max_hist = 0;
    fill(histogram, histogram + NUMBER_PATHS, 0);
    
    //create threads to create histogram
    for(int i = 0; i < n_threads; i++)
        t[i] = new thread(GeneratePuzzle, size, size, 3, Point(size - 1, 0), Point(0, size - 1), dictShape, dictNumber, i, n_threads, n_paths, tr, 1);

    //put the work in the queue
    n_states = nchoosek((size - 1) * (size - 1), 3);
    
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
        t[i] -> join();
    
    cout << "Histogram:\n";
    for(int i = 1; i <= max_hist; i++)
        cout << histogram[i] << " ";
    cout << "MAXHIST: " << max_hist << endl;*/
    
    cout << "Generating all boards...\n";
    //create threads to generate puzzles
    for(int i = 0; i < n_threads; i++)
        t[i] = new thread(GeneratePuzzle, size, size, 3, Point(size - 1, 0), Point(0, size - 1), dictShape, dictNumber, i, n_threads, n_paths, tr, 0);

    //put the work in the queue
    n_states = nchoosek((size - 1) * (size - 1), 3);
    
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
        t[i] -> join();
    
    //deleting all threads    
    for(int i = 0; i < n_threads; i++)
        delete t[i];
    
        
    //empty queue
    while(!q.IsEmpty())
        q.WaitRemove(work);
    
    cout << "TOTAL BOARDS: " << total_boards << endl;
    cout << "BOARDS WITH A SOLUTION: " << solution_boards << endl;
    cout << "BOARDS WITH DIFF # SECTIONS WITH PIECES: " << sections_boards << endl;    
    cout << "NTABLE: " << n_table << endl;
    cout << "Constructing comparison tables:...\n";
    
    //create threads
    for(int i = 0; i < n_threads; i++)
        t[i] = new thread(CreateComparisonTable, i, n_paths,n_threads);
    
    //join all threads    
    for(int i = 0; i < n_threads; i++)
        t[i] -> join();
    
    //deleting all threads    
    for(int i = 0; i < n_threads; i++)
        delete t[i];
        
    cout << "Finding best score:...\n";
    score = 0;
    
    //create threads
    for(int i = 0; i < n_threads; i++)
        t[i] = new thread(FindPuzzles, size, size, i, paths, n_paths);
    
    //put work in the queue
    n_states = nchoosek(n_table, 3);
    work_size = n_states /(n_threads * 100);
    cout << "WORK SIZE: " << work_size << endl;
    cout << "N_STATES: " << n_states << endl;
    for(uint64_t i = 0; i < n_states; i += work_size){
        work.start = i;
        work.end = i + work_size;
        
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
        t[i] -> join();
    
    //deleting all threads    
    for(int i = 0; i < n_threads; i++)
        delete t[i];
    
    //free some stuffs
    delete[] t;
    FreeTree(tr);
    
    //cout << "Best puzzle score: " << score << "\n";    
    for(int i = 0; i < 3; i++)
        cout << best_boards[i] << endl;
    
    cout << "Solution: " << paths[sol] << endl;
    
    cout << "Histogram:\n";
    for(int i = 0; i <= 15; i++)
        cout << histogram[i] << " ";
    cout << endl;
    cout << hist1 << ", " << hist2 << ", " << hist3 << endl;
    cout << "Time taken: " << timer.EndTimer() << endl;
        
    return 0;
}
