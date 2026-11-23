#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "assg.h"

#define MAX_NODES 100

/* Fixed-size storage allowed for the milestone */
Node nodes[MAX_NODES];
int node_count = 0;

Node *open_storage[MAX_NODES];
Node *closed_storage[MAX_NODES];


/* Create a new node */
Node* create_node(int x, int y, int g, int h, Node *parent) {

    if (node_count >= MAX_NODES) {
        return NULL;
    }

    Node *node = &nodes[node_count];

    node->x = x;
    node->y = y;
    node->g = g;
    node->h = h;
    node->f = g + h;
    node->parent = parent;

    node_count++;

    return node;
}


/* Compare two nodes according to heap priority */
bool node_has_higher_priority(Node *a, Node *b) {

    if (a->f != b->f) {
        return a->f < b->f;
    }

    if (a->h != b->h) {
        return a->h < b->h;
    }

    if (a->x != b->x) {
        return a->x < b->x;
    }

    return a->y < b->y;
}


/* Initialize the min-heap */
void heap_init(MinHeap *heap) {

    heap->data = open_storage;
    heap->size = 0;
    heap->capacity = MAX_NODES;
}


/* Move a node upward to maintain heap property */
void heapify_up(MinHeap *heap, int index) {

    while (index > 0) {

        int parent = (index - 1) / 2;

        if (!node_has_higher_priority(
                heap->data[index],
                heap->data[parent])) {
            break;
        }

        Node *temp = heap->data[index];
        heap->data[index] = heap->data[parent];
        heap->data[parent] = temp;

        index = parent;
    }
}


/* Insert a node into the min-heap */
void heap_insert(MinHeap *heap, Node *node) {

    if (heap->size >= heap->capacity || node == NULL) {
        return;
    }

    heap->data[heap->size] = node;
    heap->size++;

    heapify_up(heap, heap->size - 1);
}


/* Move a node downward to maintain heap property */
void heapify_down(MinHeap *heap, int index) {

    while (true) {

        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < heap->size &&
            node_has_higher_priority(
                heap->data[left],
                heap->data[smallest])) {

            smallest = left;
        }

        if (right < heap->size &&
            node_has_higher_priority(
                heap->data[right],
                heap->data[smallest])) {

            smallest = right;
        }

        if (smallest == index) {
            break;
        }

        Node *temp = heap->data[index];
        heap->data[index] = heap->data[smallest];
        heap->data[smallest] = temp;

        index = smallest;
    }
}


/* Extract the node with the smallest priority */
Node* heap_extract_min(MinHeap *heap) {

    if (heap->size == 0) {
        return NULL;
    }

    Node *min_node = heap->data[0];

    heap->size--;

    if (heap->size > 0) {

        heap->data[0] = heap->data[heap->size];

        heapify_down(heap, 0);
    }

    return min_node;
}


/* Find a node in the heap using its coordinates */
int heap_find(const MinHeap *heap, int x, int y) {

    for (int i = 0; i < heap->size; i++) {

        if (heap->data[i]->x == x &&
            heap->data[i]->y == y) {

            return i;
        }
    }

    return -1;
}


/* Decrease the priority of a node */
void heap_decrease_key(MinHeap *heap, int index,
                       int new_g, int new_h) {

    if (index < 0 || index >= heap->size) {
        return;
    }

    Node *node = heap->data[index];

    node->g = new_g;
    node->h = new_h;
    node->f = new_g + new_h;

    heapify_up(heap, index);
}


/* Check whether the heap is empty */
bool heap_is_empty(const MinHeap *heap) {

    return heap->size == 0;
}


/* Free the heap's milestone storage reference */
void heap_free(MinHeap *heap) {

    heap->size = 0;
    heap->capacity = 0;
    heap->data = NULL;
}


/* Initialize the closed list */
void closed_list_init(ClosedList *list) {

    list->data = closed_storage;
    list->size = 0;
    list->capacity = MAX_NODES;
}


/* Add a node to the closed list */
void closed_list_add(ClosedList *list, Node *node) {

    if (list->size >= list->capacity || node == NULL) {
        return;
    }

    list->data[list->size] = node;
    list->size++;
}


/* Check whether coordinates exist in the closed list */
bool closed_list_contains(const ClosedList *list, int x, int y) {

    for (int i = 0; i < list->size; i++) {

        if (list->data[i]->x == x &&
            list->data[i]->y == y) {

            return true;
        }
    }

    return false;
}


/* Free the closed list's milestone storage reference */
void closed_list_free(ClosedList *list) {

    list->size = 0;
    list->capacity = 0;
    list->data = NULL;
}


/* Remove leading and trailing whitespace */
void trim(char *str) {

    int start = 0;

    while (isspace((unsigned char)str[start])) {
        start++;
    }

    if (start > 0) {
        memmove(str, str + start, strlen(str + start) + 1);
    }

    int len = (int)strlen(str);

    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
}


/* Main milestone program */
int main(void) {

    MinHeap open_list;
    ClosedList closed_list;

    char line[256];

    heap_init(&open_list);
    closed_list_init(&closed_list);


    /* --------------------------------
       PHASE 1: INSERTION
       -------------------------------- */

    while (fgets(line, sizeof(line), stdin)) {

        trim(line);

        if (strcmp(line, "[O] Insert:") == 0) {
            break;
        }
    }


    /* Read nodes until done */
    while (fgets(line, sizeof(line), stdin)) {

        trim(line);

        if (strcmp(line, "done") == 0) {
            break;
        }

        if (strlen(line) == 0) {
            continue;
        }

        int x, y, g, h;

        if (sscanf(line, "%d , %d , %d , %d",
                   &x, &y, &g, &h) == 4) {

            Node *node = create_node(x, y, g, h, NULL);

            heap_insert(&open_list, node);
        }
    }


    /* --------------------------------
       PHASE 2: COMMANDS HEADER
       -------------------------------- */

    while (fgets(line, sizeof(line), stdin)) {

        trim(line);

        if (strcmp(line, "Commands:") == 0) {
            break;
        }
    }


    /* --------------------------------
       PHASE 3: COMMAND PROCESSING
       -------------------------------- */

    while (fgets(line, sizeof(line), stdin)) {

        trim(line);

        if (strlen(line) == 0) {
            continue;
        }

        int x, y, new_g;


        /* Open List: IsEmpty */
        if (strcmp(line, "[O] IsEmpty") == 0) {

            printf("%s\n",
                heap_is_empty(&open_list) ? "True" : "False");
        }


        /* Closed List: IsEmpty */
        else if (strcmp(line, "[C] IsEmpty") == 0) {

            printf("%s\n",
                closed_list.size == 0 ? "True" : "False");
        }


        /* Open List: Find */
        else if (sscanf(line, "[O] Find (%d, %d)",
                        &x, &y) == 2) {

            if (heap_find(&open_list, x, y) != -1) {

                printf("Node (%d, %d) has been found\n", x, y);

            } else {

                printf("Node (%d, %d) has not been found\n", x, y);
            }
        }


        /* Closed List: Find */
        else if (sscanf(line, "[C] Find (%d, %d)",
                        &x, &y) == 2) {

            if (closed_list_contains(&closed_list, x, y)) {

                printf("Node (%d, %d) has been found\n", x, y);

            } else {

                printf("Node (%d, %d) has not been found\n", x, y);
            }
        }


        /* Open List: DecreaseKey */
        else if (sscanf(line,
                        "[O] DecreaseKey (%d, %d), new_g = %d",
                        &x, &y, &new_g) == 3) {

            int index = heap_find(&open_list, x, y);

            if (index != -1) {

                int old_h = open_list.data[index]->h;

                heap_decrease_key(
                    &open_list,
                    index,
                    new_g,
                    old_h
                );

                Node *node = open_list.data[
                    heap_find(&open_list, x, y)
                ];

                printf("Decreased key for Node (%d, %d): "
                       "f = %d, g = %d, h = %d\n",
                       x, y,
                       node->f,
                       node->g,
                       node->h);
            }
        }


        /* Open List: Extract */
        else if (strcmp(line, "[O] Extract") == 0) {

            Node *node = heap_extract_min(&open_list);

            if (node != NULL) {

                closed_list_add(&closed_list, node);

                printf("Extracted Node (%d, %d), "
                       "f = %d, g = %d, h = %d\n",
                       node->x,
                       node->y,
                       node->f,
                       node->g,
                       node->h);
            }
        }
    }


    /* --------------------------------
       CLEANUP
       -------------------------------- */

    heap_free(&open_list);
    closed_list_free(&closed_list);

    return 0;
}

