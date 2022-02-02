#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//First fit
//void * ff_malloc(size_t n);
//void ff_free(void * ptr);
//Best_fit
//void * bf_malloc(size_t n, Node_t ** head, Node_t ** tail);
//void bf_free(void * ptr);
//performance study report

struct Node_t {
  size_t size;
  struct Node_t * next;
  struct Node_t * pre;
};
typedef struct Node_t Node_t;

//Thread Safe malloc/free: locking version
void * ts_malloc_lock(size_t size);
void ts_free_lock(void * ptr);

//Thread Safe malloc/free: non-locking version
void * ts_malloc_nolock(size_t size);
void ts_free_nolock(void * ptr);

void * allocate_Mem(size_t size, int lockOrNot);
void * bf_malloc(size_t n, Node_t ** head, Node_t ** tail, int lockOrNot);
void * delete_Node(Node_t * Node, Node_t ** head, Node_t ** tail);
void * split_Node(Node_t * Node, size_t n, Node_t ** head, Node_t ** tail);
void merge_Free(Node_t * Node, Node_t ** tail);
void insert_Node(Node_t * p, Node_t ** head, Node_t ** tail);
void bf_free(void * ptr, Node_t ** head, Node_t ** tail);

Node_t * first_lock = NULL;
Node_t * last_lock = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
__thread Node_t * first_nolock = NULL;
__thread Node_t * last_nolock = NULL;
