#ifndef __UNBOUNDED_LOCK_FREE_QUEUE_H__
#define __UNBOUNDED_LOCK_FREE_QUEUE_H__

struct un_lock_free_q
{
	struct queueNode *head;
	struct queueNode *tail;
};
	
struct queueNode 
{
	int postId;
	struct queueNode *next;
};

struct categories
{
	struct un_lock_free_q *lock_queue;
};

extern struct categories *categories;
int arr_size;

void queue_lock_free_init(int size);
void queue_lock_free_enq(int tId, int postId);
int queue_lock_free_deq(int tId);

// Statistics
void verify_total_queue_size(int expect_val);
void verify_total_queue_keysum(int expect_val);

#endif
