#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include "unboundedLockFreeQueue.h"

struct categories *categories = NULL;

void queue_lock_free_init(int size)
{
#if DEBUG
	fprintf(stderr, "Size = %d \n", size);
#endif
	int i = 0;
	struct queueNode *node;

	categories = malloc(size * sizeof(struct categories));

	for(i = 0; i < size; i++)
	{
		categories[i].lock_queue = malloc(sizeof(struct un_lock_free_q)); 

		node = malloc(sizeof(struct queueNode));
		node->postId = INT_MIN;
		node->next = NULL;

		categories[i].lock_queue->head = node;
		categories[i].lock_queue->tail = node;
	}

	arr_size = size;

#if DEBUG
	fprintf(stderr, "Array Size = %d \n", size);
#endif

}

void queue_lock_free_enq(int tId, int postId)
{
	struct queueNode *next, *last;
	struct queueNode *node; 
	int index = tId % arr_size;

	node = malloc(sizeof(struct queueNode));
	node->postId = postId;
	node->next = NULL;

	while (1)
	{
		last = categories[index].lock_queue->tail;
		next = last->next;

		if (last == categories[index].lock_queue->tail)
		{
			if (next == NULL)
			{
				if (__sync_bool_compare_and_swap(&last->next, next, node))
				{
					break;
				}
			}
			else 
			{
				__sync_bool_compare_and_swap(&categories[index].lock_queue->tail, last, next);
			}
		}
	}
	__sync_bool_compare_and_swap(&categories[index].lock_queue->tail, last, node);
}

int queue_lock_free_deq(int tid)
{
	struct queueNode *first, *last, *next;
	int postId;
	int index = tid % arr_size;

	while(1)
	{
		first = categories[index].lock_queue->head;
		last = categories[index].lock_queue->tail;

		next = first->next;

		if (first == categories[index].lock_queue->head)
		{
			if (first == last)
			{
				if (next == NULL)
				{
					return -1;
				}
				__sync_bool_compare_and_swap(&categories[index].lock_queue->tail, last, next);
			}
			else 
			{
				postId = next->postId;

				if (__sync_bool_compare_and_swap(&categories[index].lock_queue->head, first, next))
				{
					break;
				}
			}
		}
	}
	return postId;
}

void verify_total_queue_size(int expect_val)
{
	int i;
	int count;
	struct queueNode *tmp;
	
	for (i = 0; i < arr_size; i++)
	{
		count = 0;
		tmp = categories[i].lock_queue->head;

		while (tmp != NULL)
		{
			if (tmp->postId == INT_MIN)
			{
				tmp = tmp->next;
				continue;
			}

			count++;
			tmp = tmp->next;
		}

		printf("Categories[%d] queue's total size counted (expected: %d, found: %d)\n",
				i, expect_val, count);
	}
}

void verify_total_queue_keysum(int expect_val)
{
	int i;						// Counter
	int keysum = 0;				// Total postIds
	struct queueNode *tmp;		// Temporary pointer
	
	// Traverse the array
	for (i = 0; i < arr_size; i++)
	{
		tmp = categories[i].lock_queue->head;

		// Traverse the queue elements
		while (tmp != NULL)
		{
			if (tmp->postId == INT_MIN)
			{
				tmp = tmp->next;
				continue;
			}

			keysum += tmp->postId;
			tmp = tmp->next; }
	}

	printf("Total keysum check counted(expected: %d, found: %d)\n",
			expect_val, keysum);
}
