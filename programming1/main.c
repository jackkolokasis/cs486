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

	for(i = 0; i < num_posts; i++)
	{
		list_optcc_add(*id + i * num_threads);
	}

	pthread_barrier_wait(&barrier_1st_phase_end);

	if (*id == 0)
	{
		printf("total list size counted(expected: %d, found: %d)", 
				expected_list_size(), count_total_list_size());

		printf("total list size counted(expected: %d, found: %d)",
				expected_list_keysum(), count_total_keysum());
	}

	return NULL;
}

// How to run:
// ./phase1 -n <num_threads>
//
int main(int argc, char const* argv[])
{
	int check = 0;							   // Validation
	int i;									   // Counter
	pthread_t threads[num_threads];			   // Array of threads
	
	// Initialize number of threads and number of posts
	num_threads = atoi(argv[2]);               // Number of threads
	num_posts = atoi(argv[4]);                 // Number of threads

	check = list_optcc_init(num_posts);
	assertf(check, "Initialization Error");

	// Initialize barrier
	pthread_barrier_init(&barrier_1st_phase_end, NULL, num_threads);

	for (i = 0; i < num_threads; i++)
	{
		if (pthread_create(&threads[i], NULL, publish, (void *)&i) != 0)
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
