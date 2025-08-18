// Generic Simple doubly linked list by vincebus riveruptum. 
#pragma once

typedef struct Node{
    void *data;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct List{
    struct Node *firstNode;
    struct Node *lastNode;
    int length;
} List;
