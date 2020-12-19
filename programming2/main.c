#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "sharedDefines.h"
#include <strings.h>

#define SIZE 128		  // Maximum length of the input file name
#define BUFFER_SIZE 1024  // Maximum length of a line in input file

#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */


int main(int argc, char** argv) {
	int world_size;						// Total number of process
	int world_rank;						// Rank of the process
	char buff[BUFFER_SIZE];      		// Buffer for each line
	char event[SIZE];					// Task
	FILE *fp;							// File pointer

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);
	
	// Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	// Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// Cordinator process
	if (world_rank == 0) {

		// Open input file
		if (( fp = fopen(argv[1], "r") ) == NULL ) {
			fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
			perror("Opening test file\n");
			return EXIT_FAILURE;
		}

		// Traverse the file line by line
		while (fgets(buff, BUFFER_SIZE, fp)) {
			// Get the first token of the line
			
			sscanf(buff, "%s", event);
			DPRINT(">>>> %s\n", event);

			if (strcmp(event, "SERVER") == 0) {
				int s_rank;				// Server rank
				int l_rank;				// Left neighbour rank
				int r_rank;				// Right neighbour rank
				sscanf(buff, "%s %d %d %d", event, &s_rank, &l_rank, &r_rank);
				DPRINT("%s %d %d %d\n", event, s_rank, l_rank, r_rank);

				// send message
			}
			else if (strcmp(event, "START_LEADER_ELECTION") == 0) {
				sscanf(buff, "%s ", event);
				DPRINT("%s\n", event);
			}
			else if (strcmp(event, "CONNECT") == 0) {
				int c_rank;				// Client rank
				int p_rank;				// Parent rank
				sscanf(buff, "%s %d %d", event, &c_rank, &p_rank);
				DPRINT("%s %d %d\n", event, c_rank, p_rank);
			}
			else if (strcmp(event, "ORDER") == 0) {
				int c_rank;				// Client rank
				int num;				// Num
				sscanf(buff, "%s %d %d", event, &c_rank, &num);
				DPRINT("%s %d %d\n", event, c_rank, num);

			}
			else if (strcmp(event, "SUPPLY") == 0) {
				int s_rank;				// Server rank
				int num;				// Num 
				sscanf(buff, "%s %d %d", event, &s_rank, &num);
				DPRINT("%s %d %d\n", event, s_rank, num);

			}
			else if (strcmp(event, "PRINT") == 0) {
				sscanf(buff, "%s", event);
				DPRINT("%s\n", event);

			}
			else if (strcmp(event, "EXTERNAL_SUPPLY") == 0) {
				int num;			   // Num
				sscanf(buff, "%s %d", event, &num);
				DPRINT("%s %d\n", event, num);

			}
			else if (strcmp(event, "REPORT") == 0) {
				sscanf(buff, "%s", event);
				DPRINT("%s\n", event);
			}
			else {
				break;
			} 
		}
	}
	// Finalize the MPI environment.
	MPI_Finalize();
}
