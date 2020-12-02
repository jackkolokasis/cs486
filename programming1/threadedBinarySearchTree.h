#ifndef __THREADED_BINARY_SEARCH_TREE_H__
#define __THREADED_BINARY_SEARCH_TREE_H__

#include <pthread.h>

struct treeNode
{
	int postId;
	pthread_mutex_t lock;
	int marked;
	int is_right_threaded;
	int is_left_threaded;
	struct treeNode *lc;
	struct treeNode *rc;
};

extern struct treeNode *root;

void threaded_bs_tree_init();
void threaded_bs_tree_insert(int postId);

// Statistics
void verify_total_tree_size(int expect_val);

#endif
