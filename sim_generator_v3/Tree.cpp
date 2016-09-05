#include "Tree.h"

Tree InsertChild(Tree t, char a){
    Tree aux;
    
    if(t == NULL){
        cout << "ERROR INSERT: tree is null\n";
        return NULL;
    }
    
    aux = (Tree)malloc(sizeof(struct node));
    aux -> id = -1;
    aux -> u = aux -> d = aux -> l = aux -> r = NULL;
    aux -> a = a;
    aux -> parent = t;
    
    if(a == 'u')
        t -> u = aux;
    else if(a == 'd')
        t -> d = aux;
    else if(a == 'l')
        t -> l = aux;
    else
        t -> r = aux;
    
    return aux;
}

void RemoveChild(Tree t, char a){
    Tree aux;
    
    if(t == NULL){
        cout << "ERROR REMOVE: tree is null\n";
        return;
    }
    
    if(a == 'u')
        aux = t -> u;
    else if(a == 'd')
        aux = t -> d;
    else if(a == 'l')
        aux = t -> l;
    else
        aux = t -> r;
    
    free(aux);
}

Tree GetChild(Tree t, char a){
    if(t == NULL){
        cout << "ERROR GET: tree is null\n";
        return NULL;
    }
    
    if(a == 'u')
        return t -> u;
    else if(a == 'd')
        return t -> d;
    else if(a == 'l')
        return t -> l;
    else
        return t -> r;
}

string GetPath(Tree t){
    if(t -> a == 'X')
        return "";
    
    else
        return GetPath(t -> parent) + t -> a;
}

