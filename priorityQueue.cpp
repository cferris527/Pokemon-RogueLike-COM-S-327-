#include <cstdlib>

#include "priorityQueue.h"

int queue_init(priorityQueue *q)
{
    q->front = NULL;
    q->back = NULL;
    q->size = 0;
    return 0;
}

int queue_destroy(priorityQueue *q)
{
    queueNode *n;

    for (n = q->front; n; n = q->front) {
        q->front = n->next;
        delete n;
    }

    return 0;
}

int queue_insert(priorityQueue *q, int row, int col, double priority)
{
    queueNode* n = new queueNode;

    if (!n) {  // if null
        return -1;
    }

    n->row = row;
    n->col = col;
    n->priority = priority;
    n->next = NULL;

    if (!q->front) { // if queue empty
    q->front = q->back = n;
    }
    else {
        queueNode *current_node = q->front;

        while(current_node->next && priority <= current_node->next->priority) {
            current_node = current_node->next;

        }
        n->next = current_node->next;
        current_node->next = n;
    }
    q->size++;

    return 0;
}

int queue_front(priorityQueue *q, int *row, int *col)
{
    if (!q->front) {
    return -1;
    }

    *row = q->front->col;
    *col = q->front->row;

    return 0;
}

int queue_remove(priorityQueue *q, int *row, int *col)
{
    queueNode *n;
  
    if (!q->front) {
        return -1;
    }

    n = q->front;
    *row = n->row;
    *col = n->col;
    if (!(q->front = n->next)) {
        q->back = NULL;
    }
    q->size--;
    q->front = n->next;

    delete n;

    return 0;
}

int queue_decrease_priority(priorityQueue *q, int row, int col, double new_priority)
{
    if (!q->front) {
    return -1;
    }

    queueNode *current_node = q->front;

    while (current_node != NULL ) {
        if (current_node->row == row && current_node->col == col) {
            queue_remove(q, &row, &col);
            break;
        }
        current_node = current_node->next;
    }
    queue_insert(q, row, col, new_priority);


    return -1;
}

int queue_size(priorityQueue *q)
{
    return q->size;
}

int is_Empty(priorityQueue *q)
{
    return q->size == 0;
}