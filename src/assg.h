#ifndef ASSG_H
#define ASSG_H

#include <stdbool.h>

// Represents a node in the A* search space
typedef struct Node {
    int x;
    int y;
    int g;               // Exact path cost from start to current node
    int h;               // Heuristic estimate cost from current node to end
    int f;               // Total estimated cost (f = g + h)
    struct Node *parent; // Pointer to parent node for reconstructing the path
} Node;

// Represents the Min-Heap structure used for the Open List (priority queue)
typedef struct {
    Node **data;        // Dynamic array of Node pointers
    int size;           // Current number of elements in the heap
    int capacity;       // Maximum capacity of the heap
} MinHeap;

// Represents the Closed List structure (dynamic array)
typedef struct {
    Node **data;        // Dynamic array of Node pointers in the closed list
    int size;           // Current number of elements in the closed list
    int capacity;       // Maximum capacity of the closed list
} ClosedList;

// --- Node Constructor ---
Node* create_node(int x, int y, int g, int h, Node *parent);

// --- MinHeap (Open List) Functions ---
void heap_init(MinHeap *heap);
void heap_insert(MinHeap *heap, Node *node);
Node* heap_extract_min(MinHeap *heap);
void heapify_up(MinHeap *heap, int index);
void heapify_down(MinHeap *heap, int index);
int heap_find(const MinHeap *heap, int x, int y);
void heap_decrease_key(MinHeap *heap, int index, int new_g, int new_h);
bool heap_is_empty(const MinHeap *heap);
void heap_free(MinHeap *heap);

// --- Closed List Functions ---
void closed_list_init(ClosedList *list);
void closed_list_add(ClosedList *list, Node *node);
bool closed_list_contains(const ClosedList *list, int x, int y);
void closed_list_free(ClosedList *list);

#endif // ASSG_H
