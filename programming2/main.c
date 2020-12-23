#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "sharedDefines.h"
#include <strings.h>
#include <math.h>
#include "server.h"

#define SIZE 128		  // Maximum length of the input file name
#define BUFFER_SIZE 1024  // Maximum length of a line in input file
#define MSG_SIZE 5		  // Message size

// For the messages I use an int array.
// Mapping of each event to message type
// +-------+---------+---------+
// | event | value 1 | value 2 |
// +-------+---------+---------+
//
// +-----------------------+--------+
// | EVENT TYPE:           | VALUE  |
// +-----------------------+--------+
// | SERVER				   |    1   |
// | START_LEADER_ELECTION |    2   |
// | CONNECT               |    3   |
// | ORDER				   |    4   | 
// | SUPPLY                |    5   | 
// | PRINT                 |    6   |
// | EXTERNAL SUPPLY       |    7   |
// | REPORT                |    8   |
// | PROB                  |    9   |
// | REPLY                 |    10  |
// | TERMINATE             |    11  |
// +-----------------------+--------+

int main(int argc, char** argv) {
	int world_size;						// Total number of process
	int world_rank;						// Rank of the process
	char buff[BUFFER_SIZE];      		// Buffer for each line
	char event[SIZE];					// Task
	FILE *fp;							// File pointer
	int msg[MSG_SIZE];				            // Message to the server process
										// msg[0] = left server
										// msg[1] = right server
    char ack[SIZE];	        			// Ack 
	int *servers;						// Servers Ids
	int num_servers;					// Total number of servers
	int i;
	int leader = 0;

	MPI_Status status;

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

				sscanf(buff, "%s %d %d %d", event, &s_rank, &msg[1], &msg[2]);
				DPRINT("%s %d %d %d\n", event, s_rank, msg[1], msg[2]);

				// Server message type
				msg[0] = 1;

				servers[i++] = s_rank;
				MPI_Send(msg, MSG_SIZE, MPI_INT, s_rank, 0, MPI_COMM_WORLD);
		
				MPI_Recv(ack, SIZE, MPI_CHAR, s_rank, 0, MPI_COMM_WORLD, &status);
				DPRINT(">>>> ACK\n");

			}
			else if (strcmp(event, "START_LEADER_ELECTION") == 0) {
				sscanf(buff, "%s ", event);
				DPRINT("%s\n", event);

				for (i = 0; i < num_servers; i++) {

					// Prepare message to send
					msg[0] = 2;
					msg[1] = 0;
					msg[2] = 0;
					
					MPI_Send(msg, MSG_SIZE, MPI_INT, servers[i], 0, MPI_COMM_WORLD);
				}
				
				MPI_Recv(ack, SIZE, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
				DPRINT("%s\n", ack);

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
		while (1) {
			MPI_Recv(msg, MSG_SIZE, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

			DPRINT("msg[0] = %d | msg[1] = %d | msg[2] = %d \n", msg[0], msg[1], msg[2]);


			switch (msg[0]) {
				case 1: 
					DPRINT("[SERVER] 0 -> %d | %d | %d \n", world_rank, msg[0], msg[1]);

					server_init(world_rank, msg[1], msg[2]);
					DPRINT(">>>> %d | %d | %d\n", world_rank, msg[0], msg[1]);

					strcat(ack, "ack");
					MPI_Send(ack, SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

					break;
				case 2:
					DPRINT("[LEADER ELECTION] 0 -> %d | %d\n", world_rank, msg[0]);
					if (is_server_asleep()) {
						set_server_asleep(0);

						msg[0] = 9; 
						msg[1] = world_rank; 
						msg[2] = world_rank; 
						msg[3] = 0;
						msg[4] = 1;

						MPI_Send(msg, MSG_SIZE, MPI_CHAR, left_server_id(), 0, MPI_COMM_WORLD);
						MPI_Send(msg, MSG_SIZE, MPI_CHAR, right_server_id(), 0, MPI_COMM_WORLD);
					}

					break;

				case 8:
					break;

				case 9:
					DPRINT("[PROB] 9 -> %d | %d\n", world_rank, msg[0]);
					int sender = msg[1];
					int j = msg[2];
					int k = msg[3];
					int d = msg[4];

					if (j == world_rank) {
						msg[0] = 11;
						msg[1] = world_rank;
						msg[2] = 0;
						msg[3] = 0;
						msg[4] = 0;

						MPI_Send(msg, MSG_SIZE, MPI_CHAR, left_server_id(), 0, MPI_COMM_WORLD);
					}

					if (j > world_rank && d < pow(2, k)) {
						msg[0] = 9;
						msg[1] = world_rank;
						msg[2] = j;
						msg[3] = k;
						msg[4] = d + 1;

						if (sender == left_server_id())
							MPI_Send(msg, MSG_SIZE, MPI_CHAR, right_server_id(), 0, MPI_COMM_WORLD);
						else
							MPI_Send(msg, MSG_SIZE, MPI_CHAR, left_server_id(), 0, MPI_COMM_WORLD);
					}
					
					if (j > world_rank && d >= pow(2, k)) {
						msg[0] = 10;
						msg[1] = world_rank;
						msg[2] = j;
						msg[3] = k;
						msg[4] = 0;

						if (sender == left_server_id())
							MPI_Send(msg, MSG_SIZE, MPI_CHAR, right_server_id(), 0, MPI_COMM_WORLD);
						else
							MPI_Send(msg, MSG_SIZE, MPI_CHAR, left_server_id(), 0, MPI_COMM_WORLD);
					}
					break;

				case 10:
					sender = msg[1];
					j = msg[2];
					k = msg[3];

					if (j != world_rank) {
						msg[0] = 10;
						msg[1] = world_rank;
						msg[2] = j;
						msg[3] = k;
						msg[4] = k;

						if (sender == left_server_id()) {
							set_server_l_reply(1);
							MPI_Send(msg, MSG_SIZE, MPI_CHAR, right_server_id(), 0, MPI_COMM_WORLD);
						}
						else {
							set_server_r_reply(1);
							MPI_Send(msg, MSG_SIZE, MPI_CHAR, left_server_id(), 0, MPI_COMM_WORLD);
						}
					}
					else {
						if (is_server_l_reply() || is_server_r_reply()) {
							msg[0] = 9;
							msg[1] = world_rank;
							msg[2] = world_rank;
							msg[3] = k+1;
							msg[4] = 1;

							MPI_Send(msg, MSG_SIZE, MPI_CHAR, left_server_id(), 0, MPI_COMM_WORLD);
							MPI_Send(msg, MSG_SIZE, MPI_CHAR, right_server_id(), 0, MPI_COMM_WORLD);
						}
					}
					break;

				case 11:
					leader = msg[1];

					if (!has_leader()) {
						msg[0] = 11;
						msg[1] = leader;
						msg[2] = 0;
						msg[3] = 0;
						msg[4] = 0;

						set_leader(leader);
						MPI_Send(msg, MSG_SIZE, MPI_CHAR, left_server_id(), 0, MPI_COMM_WORLD);
					}
					else {
						strcpy(ack, "LEADER_ELECTION_DONE");
						DPRINT("LEADER = %d\n", get_leader());
						MPI_Send(ack, SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
					}
					break;
			}
		}
	}
	// Wait all process to finish their work
	MPI_Barrier(MPI_COMM_WORLD);

	// Finalize the MPI environment.
	MPI_Finalize();
	return 0;
}
