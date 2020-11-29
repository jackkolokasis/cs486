#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h> 
#include "singleLinkedList.h"
#include "sharedDefines.h"
#include "unboundedLockFreeQueue.h"

pthread_barrier_t barrier_1st_phase_end;   // Sync barrier for phase 1
pthread_barrier_t barrier_2nd_phase_end;   // Sync barrier for phase 2
pthread_barrier_t barrier_3rd_phase_start; // Sync barrier for phase 3

int num_publishers;				           // Number of threads
int num_posts;							   // Number of posts
int num_contollers;					       // Number of controllers
int num_admins;							   // Number of admins

void *blogging_system(void *args)
{
	int *id = (int *) args;
	int i = 0;
	int postId;

#if DEBUG
	fprintf(stderr,  "%s | ID = %d \n", __func__, *id);
#endif

	for(i = 0; i < num_posts; i++)
	{
		list_optcc_add(*id + i * num_publishers);
	}

	pthread_barrier_wait(&barrier_1st_phase_end);

	if (*id == 0)
	{
		verify_total_list_size(2 * pow(num_posts, 2));

		verify_total_list_keysum((2 * pow(num_posts, 4)) - pow(num_posts, 2));
	}
	
	pthread_barrier_wait(&barrier_1st_phase_end);

	// Begin second phase
	if (*id >= 0 && *id < num_admins)
	{
		for (i = 0; i < num_posts; i++)
		{
			postId = list_optcc_remove(*id + i * num_publishers);
			assertf(postId != INVALID, "Error on deletion %d %d %d", *id, i, num_publishers);
			queue_lock_free_enq(*id, postId);
			
			postId = list_optcc_remove(*id + i * num_publishers + num_posts);
			assertf(postId != INVALID, "Error on deletion %d %d %d %d", *id, i, num_publishers, num_posts);
			queue_lock_free_enq(*id, postId);
		}
	
		pthread_barrier_wait(&barrier_2nd_phase_end);

		if (*id == 0)
		{
			verify_total_queue_size(8 * num_posts);
			verify_total_queue_keysum((2 * pow(num_posts, 4)) - pow(num_posts, 2));

			verify_total_list_size(0);
		}
	}

	// Begin 3rd phase
	pthread_barrier_wait(&barrier_3rd_phase_start);

	return NULL;
}

void init(int num)
{
	// Initialize global variables
	num_posts = num;
	num_contollers = num;
	num_admins = num;
	num_publishers = 2 * num;

	// Initialize linked list
	list_optcc_init(num_posts);

	// Initialize queue
	queue_lock_free_init(num_posts / 4);

	// Initialize synchronization barriers
	pthread_barrier_init(&barrier_1st_phase_end, NULL, num_publishers);
	pthread_barrier_init(&barrier_2nd_phase_end, NULL, num_admins);
	pthread_barrier_init(&barrier_3rd_phase_start, NULL, num_publishers);
}

// How to run:
// ./phase1 -n <num_threads>
//
int main(int argc, char const* argv[])
{
	// Initialize arguments
	init(atoi(argv[2]));

	int i;						      // Counter
	int id[num_publishers];			  // Thread Ids
	pthread_t pubs[num_publishers];   // Publishers threads

	for (i = 0; i < num_publishers; i++)
	{
		id[i] = i;
		if (pthread_create(&pubs[i], NULL, blogging_system, (void *)&id[i]) != 0)
		{
			fprintf(stderr, "Unable to create thread %d\n", i);
		}
	}

#if DEBUG
	fprintf(stderr, "Created %d threads\n", num_publishers);
#endif

	for (i = 0; i < num_publishers; i++)
	{
		pthread_join(pubs[i], NULL);
	}

	pthread_barrier_destroy(&barrier_1st_phase_end);
	pthread_barrier_destroy(&barrier_2nd_phase_end);
	pthread_barrier_destroy(&barrier_3rd_phase_start);

	return 0; 
}
