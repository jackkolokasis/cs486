
#ifndef __SINGLE_LINKED_LIST_H__
#define __SINGLE_LINKED_LIST_H__

#include <pthread.h>
#include <stdlib.h>

struct Node
{
	int postId;					// Post id
	pthread_mutex_t lock;		// Mutex lock
	struct Node *next;			// Next node
};

struct single_linked_l
{
	struct Node *head;			// Head of the list
	struct Node *tail;			// Tail of the list
};

extern struct single_linked_l *linked_list;

void list_optcc_init(int num_posts);
int list_optcc_add(int postId);
int list_optcc_contains(int postId);
int list_optcc_remove(int postId);

// Statistics
void verify_total_list_size(int expect_val);
void verify_total_list_keysum(int expect_val);

#endif
