#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "sharedDefines.h"

#define SIZE 128		  // Maximum length of the input file name
#define BUFFER_SIZE 1024  // Maximum length of a line in input file
#define MSG_SIZE 5		  // Message size

int main(int argc, char** argv) {
	int num_process;					// Total number of process
	int rank;						    // Rank of the process
	char buff[BUFFER_SIZE];      		// Buffer for each line
	char event[SIZE];					// Task
	FILE *fp;							// File pointer
	int *servers;						// Servers Ids
	int num_servers;					// Total number of servers
	int i;								// Total time 
	int leader;							// Leader process
	struct _msg send_msg;				// Message
	struct _msg rcv_msg;				// Message
	MPI_Status status;					// Mpi Status

	// Initialize the MPI environment
	MPI_Init(NULL, NULL);

	// Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &num_process);

	// Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Cordinator process
	if (rank == 0) {

		// Open input file
		if (( fp = fopen(argv[1], "r") ) == NULL ) {
			fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
			perror("Opening test file\n");
			return EXIT_FAILURE;
		}
		
		num_servers = atoi(argv[2]);
		servers = malloc(num_servers * sizeof(int));
		i = 0;

		// Traverse the file line by line
		while (fgets(buff, BUFFER_SIZE, fp)) {
			// Get the first token of the line
			
			sscanf(buff, "%s", event);
			DPRINT(">>>> %s\n", event);

			if (strcmp(event, "SERVER") == 0) {
				int s_rank;				// Server rank
				int l_rank;				// Left Server rank
				int r_rank;				// Right Server rank

				sscanf(buff, "%s %d %d %d", event, &s_rank, &l_rank, &r_rank);
				DPRINT("%s %d %d %d\n", event, s_rank, l_rank, r_rank);

				send_msg = prepare_msg(0, s_rank, l_rank, r_rank, 0);

				// Add server to the array
				servers[i++] = s_rank;

				my_send(&send_msg, s_rank, SERVER);	

				MPI_Recv(&rcv_msg.pid, 1, MPI_INT, MPI_ANY_SOURCE, ACK, MPI_COMM_WORLD, &status);
				DPRINT(">>> [ACK] PID %d\n", rcv_msg.pid);
			}
			else if (strcmp(event, "START_LEADER_ELECTION") == 0) {
				DPRINT("%s\n", event);
				sscanf(buff, "%s ", event);

				for (i = 0; i < num_servers; i++) {
					// Prepare Message
					send_msg = prepare_msg(servers[i], 0, 0, 0, 0);

					my_send(&send_msg, servers[i], START_LEADER_ELECTION);
				}
					
				MPI_Recv(&rcv_msg.pid, 1, MPI_INT, MPI_ANY_SOURCE, ACK, MPI_COMM_WORLD, &status);

				DPRINT(">>> [ACK] PID %d\n", rcv_msg.pid);
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
				DPRINT("Invalid option");
				break;
			}
		} 
	}
	else {
		my_receive(&rcv_msg, rank);
	}

	if (rank == 0)
	{
		for (i = 1; i < num_process; i++) {
			send_msg = prepare_msg(0, 0, 0, 0, 0);
			my_send(&send_msg, i, EXIT);
		}
	}

	// Finalize the MPI environment.
	MPI_Finalize();
	return 0;
}
