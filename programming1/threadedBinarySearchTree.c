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

static struct treeNode * in_succ(struct treeNode *node)
{
	if (node->is_right_threaded)
	{
		return node->rc;
	}

	node = node->rc;

	while (!node->is_left_threaded)
	{
		node = node->lc;
	}

	return node;
}

static struct treeNode * in_pred(struct treeNode *node)
{
	if (node->is_left_threaded)
	{
		return node->lc;
	}

	node = node->lc;

	while (!node->is_right_threaded)
	{
		node = node->rc;
	}

	assertf(node != NULL, "Error");

	return node;
}

static void has_no_children(struct treeNode *par, struct treeNode *find)
{
	if (par == sentinel)
	{
		root->lc = NULL;
		root->rc = NULL;
	}

	else if (find == par->lc)
	{
		par->is_left_threaded = 1;
		par->lc = find->lc;
	}
	else
	{
		par->is_right_threaded = 1;
		par->rc = find->rc;
	}
}

static void has_one_children(struct treeNode *par, struct treeNode *find)
{
	struct treeNode *child;
	struct treeNode *succ;
	struct treeNode *pred;
	int side;

	while(1)
	{
		if (!find->is_left_threaded)
		{
			side = 1;
			child = find->lc;
		}
		else
		{
			side = 0;
			child = find->rc;
		}

		if (child != NULL)
		{
			pthread_mutex_lock(&child->lock);
		}

		if (validate(find, child, side))
		{
			find->marked = 1;

			if (par == sentinel)
			{
				par->lc = child;
				par->rc = child;
			}
			else if (find == par->lc)
			{
				par->lc = child;
			}
			else
			{
				par->rc = child;
			}

			// Find predecessor and successor
			succ = in_succ(find);
			pred = in_pred(find);

			if (succ)
			{
				pthread_mutex_lock(&succ->lock);
			}
			if (pred)
			{
				pthread_mutex_lock(&pred->lock);
			}

			if (!find->is_left_threaded)
			{
				pred->rc = succ;
			}
			else if (!find->is_right_threaded)
			{
				succ->lc = pred;
			}

			break;
		}

		if (child != NULL)
			pthread_mutex_unlock(&child->lock);
	}
	if (pred)
		pthread_mutex_unlock(&pred->lock);

	if (succ)
		pthread_mutex_unlock(&succ->lock);

	if (child != NULL)
		pthread_mutex_unlock(&child->lock);
}

static void has_two_children(struct treeNode *par, struct treeNode *find)
{
	struct treeNode *par_succ;
	struct treeNode *succ;

	while(1)
	{
		// We have to find the inorder successor and its parent
		par_succ = find;
		succ = find->rc;

		while (succ->lc != NULL)
		{
			par_succ = succ;
			succ = succ->lc;
		}

		pthread_mutex_lock(&par_succ->lock);
		pthread_mutex_lock(&succ->lock);

		if (validate(par_succ, succ, 1))
		{
			find->postId = succ->postId;

			if (succ->is_left_threaded && succ->is_right_threaded)
			{
				succ->marked = 1;
				has_no_children(par_succ, succ);
			}
			else {
				has_one_children(par_succ, succ);
			}

			break;
		}
		pthread_mutex_unlock(&succ->lock);
		pthread_mutex_unlock(&par_succ->lock);
	}
	pthread_mutex_unlock(&succ->lock);
	pthread_mutex_unlock(&par_succ->lock);
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
	struct treeNode *tmp = root->lc;
	struct treeNode *find = NULL;

	printf("===================================\n");
	printf("POST ID = %d\n", postId);
	printf("===================================\n");

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
		printf("TMP = %d\n", tmp->postId);
	}

	if (find != NULL)
	{
		printf ("FIND = %d | %d | %d \n", find->postId, find->marked, postId);
	}
	printf("===================================\n");
	return find != NULL && !find->marked;
}

void threaded_bs_tree_remove(int postId)
{
	struct treeNode *tmp = root->lc;
	struct treeNode *par = root;
	struct treeNode *find = NULL;

	while (tmp != NULL)
	{
		if (postId == tmp->postId)
		{
			find = tmp;
			break;
		}

		par = tmp;

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

	assertf(find != NULL, "Error");

	// Case: 2 childrens
	if (!find->is_left_threaded && !find->is_right_threaded)
	{
		has_two_children(par, find);
	}
	// Case: one children
	else if (!find->is_left_threaded)
	{
		find->marked = 1;
		has_one_children(par, find);

	}
	// Case: one children
	else if (!find->is_right_threaded)
	{
		find->marked = 1;
		has_one_children(par, find);
	}
	// Case: No children
	else {
		find->marked = 1;
		has_no_children(par, find);
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
		printf("%d \n", ptr->postId);
		ptr = inorder_successor(ptr);
	}

	printf("Tree total size finished (expected: %d , found: %d)\n",
			expect_val, count);

	//assertf(expect_val == count, "Error %d - %d", expect_val, count);
}

