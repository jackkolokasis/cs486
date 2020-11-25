#include <math.h>
#include "singleLinkedList.h"
#include "sharedDefines.h"

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

int list_optcc_init(int num_posts)
{
	// Allocate memory
	linked_list = malloc(sizeof(struct single_linked_l));
	linked_list->head = malloc(sizeof(struct Node));
	linked_list->tail = malloc(sizeof(struct Node));

	// Init head node
	linked_list->head->postId = -1;
	linked_list->head->next = linked_list->tail;

	// Init tail node
	linked_list->tail->postId = -1;
	linked_list->tail->next = NULL;

	total_nodes = 2 * pow(num_posts, 2);
	total_keysum = (2 * pow(num_posts, 4)) - pow(num_posts, 2);

	return 1;
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
				pthread_mutex_unlock(&pred->lock);
				pthread_mutex_unlock(&curr->lock);
				return 1;
			}

			pthread_mutex_unlock(&pred->lock);
			pthread_mutex_unlock(&curr->lock);
			return 0;
		}
		else 
		{
			pthread_mutex_unlock(&pred->lock);
			pthread_mutex_unlock(&curr->lock);
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
			if (curr->postId == postId)
			{
				pthread_mutex_unlock(&pred->lock);
				pthread_mutex_unlock(&curr->lock);

				return 0;
			}

			struct Node *node = malloc(sizeof(struct Node));

			node->next = curr;
			pred->next = node;

			pthread_mutex_unlock(&pred->lock);
			pthread_mutex_unlock(&curr->lock);
			return 1;
		}
		else
		{
			pthread_mutex_unlock(&pred->lock);
			pthread_mutex_unlock(&curr->lock);
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

int count_total_list_size(int expected)
{
	struct Node *tmp = linked_list->head->next;
	int count;

	while (tmp != linked_list->tail)
	{
		count++;
		tmp = tmp->next;
	}

	return count;
}

int count_total_keysum(int expected)
{
	struct Node *tmp = linked_list->head->next;
	int count;

	while (tmp != linked_list->tail)
	{
		count += tmp->postId;
		tmp = tmp->next;
	}

	return count;
}

int expected_list_size()
{
	return total_nodes;
}

int expected_list_keysum()
{
	return total_keysum;
}
