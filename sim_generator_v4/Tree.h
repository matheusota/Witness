#include <iostream>
#include <string>

using namespace std;

typedef struct node{
    int id;
    struct node *u, *d, *l, *r, *parent;
    char a;
}*Tree;

Tree InsertChild(Tree t, char a);
void RemoveChild(Tree t, char a); 
Tree GetChild(Tree t, char a);
string GetPath(Tree t); 





