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

static int validate(struct treeNode *pred, struct treeNode *curr, int side)
{
	if (side)
	{
		if (curr == NULL)
		{
			return !pred->marked && (pred->lc == NULL);
		}
		else 
		{
			return (!pred->marked && 
					!curr->marked &&
					(pred->lc == curr));
		}
	}
	else
	{
		if (curr == NULL)
		{
			return !pred->marked && pred->rc == NULL;
		}
		else 
		{
			return (!pred->marked && 
					!curr->marked &&
					(pred->rc == curr));
		}

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
		int side;

		while (ptr != NULL)
		{
			// Update parent pointer
			par = ptr;

			if (postId < ptr->postId)         //Left subtree
			{
				side = 1;
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
				side = 0;
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

		if (child != NULL)
		{
			pthread_mutex_lock(&child->lock);
		}

		if (validate(par, child, side))
		{
			// Create new node
			node = malloc(sizeof(struct treeNode));
			node->marked = 0;
			node->postId = postId;
			node->is_left_threaded = 1;
			node->is_right_threaded = 1;

			if (par == sentinel)
			{
				root->lc = node;
				root->rc = node;
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

			pthread_mutex_unlock(&par->lock);

			if (child != NULL)
			{
				pthread_mutex_unlock(&child->lock);
			}

			break;
		}
		else 
		{
			pthread_mutex_unlock(&par->lock);
			if (child != NULL)
			{
				pthread_mutex_unlock(&child->lock);
			}
			continue;
		}
	}
}

int threaded_bs_tree_search(int postId)
{
	struct treeNode *tmp = root;
	struct treeNode *find;

	while (tmp != NULL)
	{
		if (postId == tmp->postId)
		{
			find = tmp;
			break;
		}

		if (postId < tmp->postId)
		{
			if (!tmp->is_left_threaded)
			{
				tmp = tmp->lc;
			}
			else {
				break;
			}
		}
		else 
		{
			if (!tmp->is_right_threaded)
			{
				tmp = tmp->rc;
			}
			else {
				break;
			}
		}
	}

	return find != NULL && !find->marked;
}

void threaded_bs_tree_remove(int postId)
{

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

	assertf(expect_val == count, "Error %d - %d", expect_val, count);
}
