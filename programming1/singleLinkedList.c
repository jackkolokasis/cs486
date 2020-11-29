#include <math.h>
#include <limits.h>
#include "sharedDefines.h"
#include "singleLinkedList.h"

struct single_linked_l *linked_list = NULL;

int validate(struct Node *pred, struct Node *curr) 
{
	struct Node *node = linked_list->head;

	while (node->postId <= pred->postId) {
		if (node == pred) {
			return pred->next == curr;
		}
		node = node->next;
	}

	return 0;
}

void list_optcc_init(int num_posts)
{
	// Allocate memory
	linked_list = malloc(sizeof(struct single_linked_l));
	linked_list->head = malloc(sizeof(struct Node));
	linked_list->tail = malloc(sizeof(struct Node));

	// Init head node
	linked_list->head->postId = INT_MIN;
	linked_list->head->next = linked_list->tail;

	// Init tail node
	linked_list->tail->postId = INT_MAX;
	linked_list->tail->next = NULL;
}

int list_optcc_remove(int postId)
{
	while (1) 
	{
		struct Node *pred = linked_list->head;
		struct Node *curr = pred->next;

		while (curr->postId < postId)
		{
			pred = curr;
			curr = curr->next;
		}

		pthread_mutex_lock(&pred->lock);
		pthread_mutex_lock(&curr->lock);

		if (validate(pred, curr))
		{
			if (curr->postId == postId)
			{
				pred->next = curr->next;
				pthread_mutex_unlock(&curr->lock);
				pthread_mutex_unlock(&pred->lock);
				return postId;
			}

			pthread_mutex_unlock(&curr->lock);
			pthread_mutex_unlock(&pred->lock);
			return INVALID;
		}
		else 
		{
			pthread_mutex_unlock(&curr->lock);
			pthread_mutex_unlock(&pred->lock);
			continue;
		}
	}
}

int list_optcc_add(int postId)
{
	while (1)
	{
		struct Node *pred = linked_list->head;
		struct Node *curr = pred->next;

		while (curr->postId < postId)
		{
			pred = curr;
			curr = curr->next;
		}

		pthread_mutex_lock(&pred->lock);
		pthread_mutex_lock(&curr->lock);

		if (validate(pred, curr))
		{
			if (curr->postId != postId)
			{
				struct Node *node = malloc(sizeof(struct Node));

				node->postId = postId;
				node->next = curr;
				pred->next = node;

				pthread_mutex_unlock(&curr->lock);
				pthread_mutex_unlock(&pred->lock);

				return 1;
			}
		}
		else
		{
			pthread_mutex_unlock(&curr->lock);
			pthread_mutex_unlock(&pred->lock);
			continue;
		}
	}
}

int list_optcc_contains(int postId)
{
	while(1)
	{
		struct Node *pred = linked_list->head;
		struct Node *curr = pred->next;

		while (curr->postId < postId)
		{
			pred = curr;
			curr = curr->next;
		}

		pthread_mutex_lock(&pred->lock);
		pthread_mutex_lock(&curr->lock);

		if (validate(pred, curr))
		{
			if (curr->postId == postId)
			{
				pthread_mutex_unlock(&pred->lock);
				pthread_mutex_unlock(&curr->lock);

				return 1;
			}
			else {
				pthread_mutex_unlock(&pred->lock);
				pthread_mutex_unlock(&curr->lock);

				return 0;
			}
		}
		else 
		{
			pthread_mutex_unlock(&pred->lock);
			pthread_mutex_unlock(&curr->lock);
			continue;
		}
	}
}

void verify_total_list_size(int expect_val)
{
	struct Node *tmp = linked_list->head->next;
	int count = 0;

	while (tmp != linked_list->tail)
	{
		count++;
		tmp = tmp->next;
	}
		
	printf("Total list size counted(expected: %d, found: %d)\n", 
				expect_val, count);
}

void verify_total_list_keysum(int expect_val)
{
	struct Node *tmp = linked_list->head->next;
	int count = 0;

	while (tmp != linked_list->tail)
	{
		count += tmp->postId;
		tmp = tmp->next;
	}

	printf("Total keysum counted(expected: %d, found: %d)\n",
			expect_val, count);
}
