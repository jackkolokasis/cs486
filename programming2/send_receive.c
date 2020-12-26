#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "client.h"
#include "server.h"
#include "client.h"
#include "sharedDefines.h"

// Prepare a message to send with values 1 - 6
void prepare_msg(int *arr, int val1, int val2, int val3, int val4, int val5, int val6) {
	arr[0] = val1;
	arr[1] = val2;
	arr[2] = val3;
	arr[3] = val4;
	arr[4] = val5;
	arr[5] = val6;
}

// Send message `msg` to process `rank` with the certain `tag`
void my_send(int *msg, int rank, int tag) {
	switch (tag) {

	case SERVER:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;

	case START_LEADER_ELECTION:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;

	case CONNECT:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;

	case EXIT:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;
	}
}

// Process `rank` receive message `msg`. Based on the tag value perform the
// following actions
void my_receive(int *msg, int rank) {
	MPI_Status status;				// Tag status

	int rcv_msg[MSG_SIZE];			// Receive message
	int send_msg[MSG_SIZE];			// Send message
	int send_msg_l[MSG_SIZE];		// Send message to the left server
	int send_msg_r[MSG_SIZE];		// Send message to the right server
	int ack_msg[MSG_SIZE];			// Ack message

	int jl, kl, dl;				    // Id, Phase, Distance for left server
	int jr, kr, dr;				    // Id, Phase, Distance for left server
	int client1, client2;			// Clients

	while(1) {
		MPI_Recv(&rcv_msg, MSG_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		switch (status.MPI_TAG) {
			case SERVER:
				DPRINT("[SERVER] | PID %d | S_RANK %d | L_RANK %d | R_RANK %d\n",
						rcv_msg[0], rcv_msg[1], rcv_msg[2],  rcv_msg[3]);

				// Initialize server
				init_server(rcv_msg[1], rcv_msg[2], rcv_msg[3]);

				prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
				MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);

				break;

			case START_LEADER_ELECTION:
				//if (is_server_asleep()) {
				//	set_server_asleep(0);
				DPRINT("==============================================\n");
				DPRINT("Processor = %d\n", rank);
				DPRINT("==============================================\n");

					// Prepare msg
					prepare_msg(send_msg, rank, 0, 1, rank, 0, 1);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, left_server_id(), PROB_LEFT, MPI_COMM_WORLD);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, right_server_id(), PROB_RIGHT, MPI_COMM_WORLD);
				//}
				break;

			case PROB_LEFT:
				jl = rcv_msg[0]; 
				kl = rcv_msg[1];
				dl = rcv_msg[2];

				if (jl == rank) {
					// Prepare msg
					prepare_msg(send_msg, rank, 0, 0, 0 , 0, 0);
					set_leader(rank);
					make_leader();
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, right_server_id(), LEADER, MPI_COMM_WORLD);
				}

				if (jl > rank && dl < pow(2, kl)) { // forward the msg
					// Prepare message to send, increment hop counter
					prepare_msg(send_msg_l, jl, kl, dl + 1, 0, 0, 0);

					MPI_Send(send_msg_l, MSG_SIZE, MPI_INT, right_server_id(), PROB_LEFT, MPI_COMM_WORLD);
				}
				if (jl > rank && dl >= pow(2, kl)) { // reply to the msg
					// Prepare message to send, increment hop counter
					prepare_msg(send_msg_l, jl, kl, 0, 0, 0, 0);
					
					// Send to the left or right
					MPI_Send(send_msg_l, MSG_SIZE, MPI_INT, left_server_id(), REPLY_LEFT, MPI_COMM_WORLD);
				}

				break;

			case PROB_RIGHT:
				jr = rcv_msg[0];
				kr = rcv_msg[1];
				dr = rcv_msg[2];

				if (jr == rank) {
					// Prepare msg
					prepare_msg(send_msg, rank, 0, 0, 0 ,0, 0);
					set_leader(rank);
					make_leader();
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, right_server_id(), LEADER, MPI_COMM_WORLD);
				}

				if (jr > rank && dr < pow(2, kr)) { // forward the msg
					// Prepare message to send, increment hop counter
					prepare_msg(send_msg_r, jr, kr, dr+1, 0, 0, 0);

					MPI_Send(send_msg_r, MSG_SIZE, MPI_INT, left_server_id(), PROB_RIGHT, MPI_COMM_WORLD);
				}
				if (jr > rank && dr >= pow(2, kr)) { // reply to the msg
					// Prepare message to send, increment hop counter
					prepare_msg(send_msg_r, jr, kr, 0, 0, 0, 0);
					
					MPI_Send(send_msg_r, MSG_SIZE, MPI_INT, right_server_id(), REPLY_RIGHT, MPI_COMM_WORLD);
				}

				break;

			case REPLY_LEFT:
				jl = rcv_msg[0];
				kl = rcv_msg[1];
				
				if (jl != rank) { // Forward the reply
					// Prepare message to send, increment hop counter
					prepare_msg(send_msg_l, jl, kl, 0, 0, 0, 0);

					MPI_Send(send_msg_l, MSG_SIZE, MPI_INT, right_server_id(), REPLY_LEFT, MPI_COMM_WORLD);
				}
				else { // Reply is for own prob
					set_server_l_reply(1);

					// Phase k winner
					if (is_server_r_reply()) {
						set_server_l_reply(0);
						set_server_r_reply(0);

						// Prepare message to send, increment hop counter
						prepare_msg(send_msg_l, rank, kl + 1, 1, 0, 0, 0);
						prepare_msg(send_msg_r, rank, kl + 1, 1, 0, 0, 0);

						MPI_Send(send_msg_l, MSG_SIZE, MPI_INT, left_server_id(), PROB_LEFT, MPI_COMM_WORLD);
						MPI_Send(send_msg_r, MSG_SIZE, MPI_INT, right_server_id(), PROB_RIGHT, MPI_COMM_WORLD);
					}
				}
				break;
			
			case REPLY_RIGHT:
				jr = rcv_msg[0];
				kr = rcv_msg[1];;
				
				if (jr != rank) { // Forward the reply
					// Prepare message to send, increment hop counter
					prepare_msg(send_msg_r, jr, kr, 0, 0, 0, 0);

					MPI_Send(send_msg_r, MSG_SIZE, MPI_INT, left_server_id(), REPLY_RIGHT, MPI_COMM_WORLD);
				}
				else { // Reply is for own prob
					set_server_r_reply(1);

					// Phase k winner
					if (is_server_l_reply()) {
						set_server_l_reply(0);
						set_server_r_reply(0);
						// Prepare message to send, increment hop counter
						prepare_msg(send_msg_l, rank, kr + 1, 1, 0, 0, 0);
						prepare_msg(send_msg_r, rank, kr + 1, 1, 0, 0, 0);

						MPI_Send(&send_msg_l, MSG_SIZE, MPI_INT, left_server_id(), PROB_LEFT, MPI_COMM_WORLD);
						MPI_Send(&send_msg_r, MSG_SIZE, MPI_INT, right_server_id(), PROB_RIGHT, MPI_COMM_WORLD);
					}
				}
				break;

			case LEADER:
				set_leader(rcv_msg[0]);

				if (is_leader()) {
					prepare_msg(send_msg, get_leader(), 0, 0, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);
				}
				else {
					prepare_msg(send_msg_l, rcv_msg[0], 0, 0, 0, 0, 0);
					MPI_Send(send_msg_l, MSG_SIZE, MPI_INT, right_server_id(), LEADER, MPI_COMM_WORLD);
				}

				break;

			case CONNECT:
				client1 = rcv_msg[0];
				client2 = rcv_msg[1];

				DPRINT("[CONNECT] %d -> %d\n", client1, client2);

				if (!client_alive())
					// Create new client - client1
					new_client(client1);

				// Add client2 to the list of neighbors of client1
				insert_nbr_client(client2);

				// Prepare message
				prepare_msg(send_msg, client1, client2, 0, 0, 0, 0);

				MPI_Send(send_msg, MSG_SIZE, MPI_INT, client2, NEIGHBOR, MPI_COMM_WORLD);

				break;

			case NEIGHBOR:

				client1 = rcv_msg[0];
				client2 = rcv_msg[1];

				DPRINT("[NEIGHBOR] %d -> %d\n", client1, client2);

				if (!client_alive())
					// Create new client - client1
					new_client(client2);

				// Add client1 to the list of neighbors of client2
				insert_nbr_client(client1);

				// Prepare ack message
				prepare_msg(send_msg, client1, client2, 0, 0, 0, 0); 
				MPI_Send(send_msg, MSG_SIZE, MPI_INT, client1, ACK, MPI_COMM_WORLD);

				break;

			case ACK:
				DPRINT("[ACK] %d -> %d\n", rcv_msg[0], rcv_msg[1]);
				
				// Send ack message to client 1
				prepare_msg(ack_msg, rcv_msg[0], rcv_msg[1], 0, 0, 0, 0);
				MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);

				break;

			case EXIT:
				if (rank > 4)
					print_client();

				return;
				break;
		}
	}
}
