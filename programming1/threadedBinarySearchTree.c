#include "threadedBinarySearchTree.h"
#include "sharedDefines.h"
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

struct treeNode *root = NULL;
struct treeNode *sentinel = NULL;

void threaded_bs_tree_init()
{
	sentinel = malloc(sizeof(struct treeNode));

	sentinel->postId = INT_MIN;
	sentinel->marked = 0;
	sentinel->is_left_threaded = 0;
	sentinel->is_right_threaded = 0;
	sentinel->lc = NULL;
	sentinel->rc = NULL;

	root = sentinel;
}

static int validate(struct treeNode *pred, struct treeNode *curr)
{
	if (curr == NULL)
	{
		return !pred->marked;
	}
	else 
	{
		return (!pred->marked && 
				!curr->marked &&
				(pred->lc == curr || pred->rc == curr));
	}
}

void threaded_bs_tree_insert(int postId)
{
	while (1)
	{
		struct treeNode *ptr =  root->lc;
		struct treeNode *par = root;
		struct treeNode *child = NULL;
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
					child = ptr->lc;
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
					child = ptr->rc;
					break;
				}
			}
		}

		pthread_mutex_lock(&par->lock);

		if (par == sentinel)
		{
			// Create new node
			node = malloc(sizeof(struct treeNode));
			node->marked = 0;
			node->postId = postId;
			node->is_left_threaded = 1;
			node->is_right_threaded = 1;

			root->lc = node;
			root->rc = node;
			node->lc = NULL;
			node->rc = NULL;

			pthread_mutex_unlock(&par->lock);
			return;
		}

		if (child != NULL)
		{
			pthread_mutex_lock(&child->lock);
		}

		if (validate(par, child))
		{
			// Create new node
			node = malloc(sizeof(struct treeNode));
			node->marked = 0;
			node->postId = postId;
			node->is_left_threaded = 1;
			node->is_right_threaded = 1;

			if (postId < (par->postId))
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

			if (child != NULL)
			{
				pthread_mutex_unlock(&child->lock);
			}
			pthread_mutex_unlock(&par->lock);

			break;
		}
		else 
		{
			if (child != NULL)
			{
				pthread_mutex_unlock(&child->lock);
			}
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
