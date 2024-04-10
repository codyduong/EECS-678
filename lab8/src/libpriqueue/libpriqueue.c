/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"

/**
  Initializes the priqueue_t data structure.

  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int (*comparer)(const void *, const void *))
{
  q->head = NULL;
  q->size = 0;
  q->comparer = comparer;
}

/**
  Insert the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  new_node->data = ptr;
  new_node->next = NULL;

  if (q->head == NULL || q->comparer(ptr, q->head->data) < 0)
  {
    new_node->next = q->head;
    q->head = new_node;
    q->size++;
    return 0; // Inserted at the front
  }

  node_t *current = q->head;
  int index = 0;
  while (current->next != NULL && q->comparer(ptr, current->next->data) >= 0)
  {
    current = current->next;
    index++;
  }

  new_node->next = current->next;
  current->next = new_node;
  q->size++;

  return index + 1;
}

/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
  if (q->head == NULL)
    return NULL;
  return q->head->data;
}

/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
  if (q->head == NULL)
    return NULL;

  node_t *temp = q->head;
  void *data = temp->data;
  q->head = q->head->next;
  free(temp);
  q->size--;

  return data;
}

/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
  if (index < 0 || index >= q->size)
    return NULL;

  node_t *current = q->head;
  for (int i = 0; i < index; i++)
  {
    current = current->next;
  }
  return current->data;
}

/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
  int count = 0;
  node_t **current = &q->head;

  while (*current != NULL)
  {
    node_t *entry = *current;
    if (entry->data == ptr)
    {
      *current = entry->next;
      free(entry);
      q->size--;
      count++;
    }
    else
    {
      current = &entry->next;
    }
  }

  return count;
}

/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
  if (index < 0 || index >= q->size)
    return NULL;

  node_t **current = &q->head;
  for (int i = 0; i < index; i++)
  {
    current = &(*current)->next;
  }

  node_t *temp = *current;
  void *data = temp->data;
  *current = temp->next;
  free(temp);
  q->size--;

  return data;
}

/**
  Return the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
  return q->size;
}

/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
  node_t *current = q->head;
  while (current != NULL)
  {
    node_t *next = current->next;
    free(current);
    current = next;
  }
  q->head = NULL;
  q->size = 0;
}
