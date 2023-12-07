#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

/**
 * Priority Queue data structure.
 * Implemented in NPC pathfinding.
*/
struct queueNode {
  queueNode *next;
  int row;
  int col;
  double priority;
};

struct priorityQueue {
  queueNode *front, *back;
  int size;
};

int queue_init(priorityQueue *q);
int queue_destroy(priorityQueue *q);
int queue_insert(priorityQueue *q, int row, int col, double priority);
int queue_front(priorityQueue *q, int *row, int *col);
int queue_remove(priorityQueue *q, int *row, int *col);
int queue_decrease_priority(priorityQueue *q, int row, int col, double new_priority);
int queue_size(priorityQueue *q);
int is_Empty(priorityQueue *q);

#endif