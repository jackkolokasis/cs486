#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "client.h"
#include "server.h"
#include "client.h"
#include "sharedDefines.h"

struct _msg prepare_msg(int pid, int val_1, int val_2, int val_3, int val_4) {
	struct _msg msg;

	msg.pid = pid; 
	msg.val_1 = val_1; 
	msg.val_2 = val_2; 
	msg.val_3 = val_3; 

	return msg;
}

void my_send(struct _msg *msg, int rank, int tag) {
	switch (tag) {

	case SERVER:
		MPI_Send(&msg->pid, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
		MPI_Send(&msg->val_1, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
		MPI_Send(&msg->val_2, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
		MPI_Send(&msg->val_3, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
		MPI_Send(&msg->val_4, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;

	case START_LEADER_ELECTION:

		MPI_Send(&msg->pid, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);

		break;

	case CONNECT:
		MPI_Send(&msg->pid, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
		MPI_Send(&msg->val_1, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);

		break;

	case EXIT:
		MPI_Send(&msg->pid, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;
	}
}

void my_receive(struct _msg *msg, int rank) {
	MPI_Status status;				// Tag status
	struct _msg rcv_msg;			// Receive message
	struct _msg send_msg;			// Ack message
	struct _msg ack_msg;			// Ack message
	int j, k, d;				    // Ids for leader election
	int client1, client2;			// Clients

	while(1) {
		MPI_Recv(&rcv_msg.pid, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		switch (status.MPI_TAG) {
			case SERVER:
				MPI_Recv(&rcv_msg.val_1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&rcv_msg.val_2, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&rcv_msg.val_3, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&rcv_msg.val_4, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				DPRINT("[SERVER] | PID %d | S_RANK %d | L_RANK %d | R_RANK %d\n",
						rcv_msg.pid, rcv_msg.val_1, rcv_msg.val_2, 
						rcv_msg.val_3);

				// Initialize server
				init_server(rcv_msg.val_1, rcv_msg.val_2, rcv_msg.val_3);

				ack_msg = prepare_msg(rcv_msg.val_1, 0, 0, 0, 0);

				MPI_Send(&ack_msg.pid, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD);

				break;

			case START_LEADER_ELECTION:
				//if (is_server_asleep()) {
				//	set_server_asleep(0);

					// Prepare msg
					send_msg = prepare_msg(rank, 0, 1, 0, 0);

					MPI_Send(&send_msg.pid, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
					MPI_Send(&send_msg.val_1, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
					MPI_Send(&send_msg.val_2, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);

					MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
					MPI_Send(&send_msg.val_1, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
					MPI_Send(&send_msg.val_2, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
				//}
				break;

			case PROB:
				MPI_Recv(&rcv_msg.val_1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&rcv_msg.val_2, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				j = rcv_msg.pid;
				k = rcv_msg.val_1;
				d = rcv_msg.val_2;

				if (j == rank) {
					// Prepare msg
					send_msg = prepare_msg(rank, 0, 0, 0 ,0);
					set_leader(rank);
					make_leader();
					MPI_Send(&send_msg.pid, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD);
					//MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), LEADER, MPI_COMM_WORLD);
				}

				if (j > rank && d < pow(2, k)) { // forward the msg
					// Prepare message to send, increment hop counter
					send_msg = prepare_msg(j, k, d+1, 0, 0);

					// Send to the left or right
					if (status.MPI_SOURCE == left_server_id()) {
						MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_2, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
					}

					if (status.MPI_SOURCE == right_server_id()) {
						MPI_Send(&send_msg.pid, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_2, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
					}
				}
				if (j > rank && d >= pow(2, k)) { // reply to the msg
					// Prepare message to send, increment hop counter
					send_msg = prepare_msg(j, k, 0, 0, 0);
					
					// Send to the left or right
					if (status.MPI_SOURCE == left_server_id()) {
						MPI_Send(&send_msg.pid, 1, MPI_INT, left_server_id(), REPLY, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, left_server_id(), REPLY, MPI_COMM_WORLD);
					}

					if (status.MPI_SOURCE == right_server_id()) {
						MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), REPLY, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, right_server_id(), REPLY, MPI_COMM_WORLD);
					}
				}

				break;

			case REPLY:
				MPI_Recv(&rcv_msg.val_1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				
				j = rcv_msg.pid;
				k = rcv_msg.val_1;
				
				if (j != rank) { // Forward the reply
					// Prepare message to send, increment hop counter
					send_msg = prepare_msg(j, k, 0, 0, 0);

					// Send to the left or right
					if (status.MPI_SOURCE == left_server_id()) {
						MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), REPLY, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, right_server_id(), REPLY, MPI_COMM_WORLD);
					}

					if (status.MPI_SOURCE == right_server_id()) {
						MPI_Send(&send_msg.pid, 1, MPI_INT, left_server_id(), REPLY, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, left_server_id(), REPLY, MPI_COMM_WORLD);
					}
				}
				else { // Reply is for own prob
					// Prepare message to send, increment hop counter
					if (status.MPI_SOURCE == left_server_id())
						set_server_l_reply(1);
					else 
						set_server_r_reply(1);

					// Phase k winner
					if (is_server_l_reply() && is_server_r_reply()) {
						set_server_l_reply(0);
						set_server_r_reply(0);
						send_msg = prepare_msg(rank, k + 1, 1, 0, 0);

						MPI_Send(&send_msg.pid, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_2, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);

						MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_2, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
					}
				}
				break;


			case LEADER:
				set_leader(rcv_msg.pid);

				if (is_leader())
					MPI_Send(&rcv_msg.pid, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD);
				else {
					MPI_Send(&rcv_msg.pid, 1, MPI_INT, right_server_id(), LEADER, MPI_COMM_WORLD);
				}

				break;

			case CONNECT:
				MPI_Recv(&rcv_msg.val_1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				client1 = rank;
				client2 = rcv_msg.val_1;

				DPRINT("[CONNECT] %d -> %d\n", client1, client2);

				if (!client_alive())
					// Create new client - client1
					new_client(client1);

				// Add client2 to the list of neighbors of client1
				insert_nbr_client(client2);

				MPI_Send(&rcv_msg.val_1, 1, MPI_INT, client2, NEIGHBOR, MPI_COMM_WORLD);
				MPI_Send(&rcv_msg.pid, 1, MPI_INT, client2, NEIGHBOR, MPI_COMM_WORLD);
				break;

			case NEIGHBOR:
				MPI_Recv(&rcv_msg.val_1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				
				client1 = rcv_msg.val_1;
				client2 = rank;

				DPRINT("[NEIGHBOR] %d -> %d\n", client1, client2);

				if (!client_alive())
					// Create new client - client1
					new_client(client2);

				// Add client1 to the list of neighbors of client2
				insert_nbr_client(client1);

				MPI_Send(&rcv_msg.val_1, 1, MPI_INT, client2, NEIGHBOR, MPI_COMM_WORLD);
				
				// Send ack message to client 1
				ack_msg = prepare_msg(rank, 0, 0, 0, 0);
				MPI_Send(&ack_msg.pid, 1, MPI_INT, client1, ACK, MPI_COMM_WORLD);

				break;

			case ACK:
				DPRINT("[ACK] %d -> %d", rcv_msg.pid, rank);
				
				// Send ack message to client 1
				ack_msg = prepare_msg(rank, 0, 0, 0, 0);
				MPI_Send(&ack_msg.pid, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD);

				break;

			case EXIT:
				return;
				break;
		}
	}
}
