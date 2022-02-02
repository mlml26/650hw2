#include "my_malloc.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//use sbrk function to allocate memory of size
void * allocate_Mem(size_t size, int lockOrNot) {
  unsigned long required = size + sizeof(Node_t);
  Node_t * new_Node = NULL;
  if (lockOrNot == 0) {
    new_Node = sbrk(required);
  }
  else {
    pthread_mutex_lock(&lock);
    new_Node = sbrk(required);
    pthread_mutex_unlock(&lock);
  }
  new_Node->size = size;
  new_Node->next = NULL;
  new_Node->pre = NULL;
  return (void *)new_Node + sizeof(Node_t);
}
void * delete_Node(Node_t * Node, Node_t ** head, Node_t ** tail) {
  //Node->size==n:delete the Node block
  if (!Node->pre && !Node->next) {
    *head = *tail = NULL;
  }
  else if (!Node->pre) {  //head
    Node->next->pre = NULL;
    *head = Node->next;
  }
  else if (!Node->next) {  //tail
    Node->pre->next = NULL;
    *tail = Node->pre;
  }
  else {
    Node->pre->next = Node->next;
    Node->next->pre = Node->pre;
  }
  Node->next = NULL;
  Node->pre = NULL;

  return (void *)Node + sizeof(Node_t);
}
void * split_Node(Node_t * Node, size_t n, Node_t ** head, Node_t ** tail) {
  //Node->size> n+ sizeof(Node_t):split the Node into Node and remaining block
  Node_t * remaining;
  remaining = (Node_t *)((void *)Node + sizeof(Node_t) + n);
  remaining->size = Node->size - n - sizeof(Node_t);
  if (!Node->pre && !Node->next) {
    *head = *tail = remaining;
  }
  else if (!Node->pre) {  //head
    Node->next->pre = remaining;
    *head = remaining;
  }
  else if (!Node->next) {  //tail
    Node->pre->next = remaining;
    *tail = remaining;
  }
  else {  //middle
    Node->next->pre = remaining;
    Node->pre->next = remaining;
  }
  remaining->next = Node->next;
  remaining->pre = Node->pre;

  //release the Node(size=n)
  Node->size = n;
  Node->next = NULL;
  Node->pre = NULL;

  return (void *)Node + sizeof(Node_t);
}

void * bf_malloc(size_t n, Node_t ** head, Node_t ** tail, int lockOrNot) {
  Node_t * cur = *head;
  Node_t * bf = NULL;
  while (cur) {
    if (cur->size == n) {
      bf = cur;
      return delete_Node(cur, head, tail);
    }
    else if (cur->size > n + sizeof(Node_t)) {
      if (bf == NULL || (bf && bf->size > cur->size)) {
        bf = cur;
      }
      cur = cur->next;
    }
    else
      cur = cur->next;
  }
  if (bf) {
    return split_Node(bf, n, head, tail);
  }
  else
    return allocate_Mem(n, lockOrNot);
}

//Thread Safe malloc: locking version
void * ts_malloc_lock(size_t size) {
  pthread_mutex_lock(&lock);
  void * ptr = bf_malloc(size, &first_lock, &last_lock, 0);
  pthread_mutex_unlock(&lock);
  return ptr;
}

//Thread Safe malloc: non-locking version
void * ts_malloc_nolock(size_t size) {
  void * ptr = bf_malloc(size, &first_nolock, &last_nolock, 1);
  return ptr;
}

void insert_Node(Node_t * p, Node_t ** head, Node_t ** tail) {
  //base case1: if there is no free list;
  if (!*head) {
    *head = *tail = p;
    return;
  }
  //base case2: if the head node is after cur
  if (*head > p) {
    p->next = *head;
    (*head)->pre = p;
    *head = p;
    return;
  }
  Node_t * cur = *head;
  //searching for the first node after p
  while (cur && cur < p)
    cur = cur->next;
  //if there is no node after p: connect tail and p
  if (!cur) {
    (*tail)->next = p;
    p->pre = *tail;
    *tail = p;
  }
  //if p is between two node: cur->pre and cur
  else {
    p->pre = cur->pre;
    cur->pre->next = p;
    p->next = cur;
    cur->pre = p;
  }
}

void merge_Free(Node_t * Node, Node_t ** tail) {
  //if (!Node)
  //  return;
  Node_t * before = Node->pre;
  Node_t * after = Node->next;
  //merge with the next after node
  if (after && (void *)Node + Node->size + sizeof(Node_t) == (void *)after) {
    Node->size = Node->size + Node->next->size + sizeof(Node_t);
    Node->next = after->next;
    //
    if (Node->next)
      Node->next->pre = Node;
    else
      *tail = Node;
  }
  else if (before && (void *)before + before->size + sizeof(Node_t) == (void *)Node) {
    before->size = Node->size + before->size + sizeof(Node_t);
    before->next = Node->next;
    if (before->next) {
      before->next->pre = before;
    }
    else
      *tail = before;
  }
}

void bf_free(void * ptr, Node_t ** head, Node_t ** tail) {
  Node_t * p = NULL;
  p = (Node_t *)(ptr - sizeof(Node_t));
  p->next = NULL;
  p->pre = NULL;
  //1.if there is no free list
  insert_Node(p, head, tail);
  merge_Free(p, tail);
}

//free
void ts_free_lock(void * ptr) {
  pthread_mutex_lock(&lock);
  bf_free(ptr, &first_lock, &last_lock);
  pthread_mutex_unlock(&lock);
}
void ts_free_nolock(void * ptr) {
  bf_free(ptr, &first_nolock, &last_nolock);
}
