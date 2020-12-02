#include "threadedBinarySearchTree.h"
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

struct treeNode *root = NULL;

void threaded_bs_tree_init()
{
	root = malloc(sizeof(struct treeNode));

	root->postId = INT_MIN;
	root->marked = 0;
	root->is_left_threaded = 0;
	root->is_right_threaded = 0;
	root->lc = NULL;
	root->rc = NULL;
}

static int validate(struct treeNode *pred, struct treeNode *curr)
{
	return (!pred->marked && 
			!curr->marked &&
			(pred->lc == curr || pred->rc == curr));
}

void threaded_bs_tree_insert(int postId)
{
	while (1)
	{
		struct treeNode *ptr =  root;
		struct treeNode *par = NULL;
		struct treeNode *node = NULL;

		while (ptr != NULL)
		{
			// Update parent pointer
			par = ptr;

			if (postId < ptr->postId)         //Left subtree
			{
				if (!ptr->is_left_threaded)
				{
					ptr = ptr->lc;
				}
				else 
				{
					break;
				}
			}
			else							 // Right subtree 
			{
				if (!ptr->is_right_threaded)
				{
					ptr = ptr->rc;
				}
				else 
				{
					break;
				}
			}
		}

		pthread_mutex_lock(&ptr->lock);
		pthread_mutex_lock(&par->lock);

		if (validate(par, ptr))
		{
			// Create new node
			node = malloc(sizeof(struct treeNode));
			node->marked = 0;
			node->postId = postId;
			node->is_left_threaded = 1;
			node->is_right_threaded = 1;

			if (par == NULL)
			{
				root = node;
				node->lc = NULL;
				node->rc = NULL;
			}
			else if (postId < (par->postId))
			{
				node->lc = par->lc;
				node->rc = par;
				par->is_left_threaded = 0;
				par->lc = node;
			}
			else 
			{
				node->lc = par;
				node->rc = par->rc;
				par->is_right_threaded = 0;
				par->rc = node;
			}

			pthread_mutex_unlock(&ptr->lock);
			pthread_mutex_unlock(&par->lock);
		}
		else 
		{
			pthread_mutex_unlock(&ptr->lock);
			pthread_mutex_unlock(&par->lock);
			continue;
		}
	}
}

struct treeNode *inorder_successor(struct treeNode *ptr)
{
	if (ptr->is_right_threaded) 
	{
		return ptr->rc;
	}

	ptr = ptr->rc;
	while (!ptr->is_left_threaded)
	{
		ptr = ptr->lc;
	}

	return ptr;
}

void verify_total_tree_size(int expect_val)
{
	int count = 0;
	struct treeNode *ptr;

	if (root->lc == NULL)
	{
		printf("Tree total size finished (expected: %d , found: %d)\n",
				expect_val, count);
		return;
	}

	ptr = root->lc;

	while (!ptr->is_left_threaded)
	{
		ptr = ptr->lc;
	}

	while (ptr != NULL)
	{
		count++;
		ptr = inorder_successor(ptr);
	}

	printf("Tree total size finished (expected: %d , found: %d)\n",
			expect_val, count);
}
