#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h> 
#include "singleLinkedList.h"
#include "sharedDefines.h"

pthread_barrier_t barrier_1st_phase_end;   // Sync barrier
int num_threads;				           // Number of threads
int num_posts;							   // Number of posts

void *publish(void *args)
{
	int *id = (int *) args;
	int i = 0;

#if DEBUG
	fprintf(stderr,  "%s | ID = %d \n", __func__, *id);
#endif

	for(i = 0; i < num_posts; i++)
	{
		list_optcc_add(*id + i * num_threads);
	}

	pthread_barrier_wait(&barrier_1st_phase_end);

	if (*id == 0)
	{
		printf("total list size counted(expected: %d, found: %d)\n", 
				expected_list_size(), count_total_list_size());

		printf("total keysum counted(expected: %d, found: %d)\n",
				expected_list_keysum(), count_total_keysum());
	}

	return NULL;
}

// How to run:
// ./phase1 -n <num_threads>
//
int main(int argc, char const* argv[])
{
	// Initialize number of threads and number of posts
	num_posts = atoi(argv[2]);                 // Number of threads
	num_threads = 2 * num_posts;              // Number of threads
	pthread_t threads[num_threads];			   // Array of threads

	int i;									   // Counter
	int id[num_threads];					   // Thread Ids

	list_optcc_init(num_posts);

	// Initialize barrier
	pthread_barrier_init(&barrier_1st_phase_end, NULL, num_threads);

	for (i = 0; i < num_threads; i++)
	{
		id[i] = i;
		if (pthread_create(&threads[i], NULL, publish, (void *)&id[i]) != 0)
		{
			fprintf(stderr, "Unable to create thread %d\n", i);
		}
	}

#if DEBUG
	fprintf(stderr, "Created %d threads\n", num_threads);
#endif

	for (i = 0; i < num_threads; i++)
	{
		pthread_join(threads[i], NULL);
	}

	pthread_barrier_destroy(&barrier_1st_phase_end);

	return 0; 
}
