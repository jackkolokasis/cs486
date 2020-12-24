#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "server.h"
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

	while(1) {
		MPI_Recv(&rcv_msg.pid, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		printf("status.MPI_TAG = %d\n", status.MPI_TAG);
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
				if (is_server_asleep()) {
					set_server_asleep(0);

					// Prepare msg
					send_msg = prepare_msg(rank, 0, 1, 0, 0);

					MPI_Send(&send_msg.pid, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
					MPI_Send(&send_msg.val_1, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
					MPI_Send(&send_msg.val_2, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);

					MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
					MPI_Send(&send_msg.val_1, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
					MPI_Send(&send_msg.val_2, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
				}
				break;

			case PROB:
				MPI_Recv(&rcv_msg.val_1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&rcv_msg.val_2, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				if (has_leader())
					break;

				j = rcv_msg.pid;
				k = rcv_msg.val_1;
				d = rcv_msg.val_2;

				if (j == rank) {
					// Prepare msg
					send_msg = prepare_msg(rank, 0, 0, 0 ,0);
					set_leader(rank);
					MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), LEADER, MPI_COMM_WORLD);
				}

				if (j > rank && d < pow(2, k)) { // forward the msg
					// Prepare message to send, increment hop counter
					send_msg = prepare_msg(j, k, d+1, 0, 0);

					// Send to the left or right
					if (status.MPI_SOURCE == left_server_id()) {
						set_server_asleep(0);
						MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_2, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
					}

					if (status.MPI_SOURCE == right_server_id()) {
						set_server_asleep(0);
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
						set_server_asleep(0);
						MPI_Send(&send_msg.pid, 1, MPI_INT, left_server_id(), REPLY, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, left_server_id(), REPLY, MPI_COMM_WORLD);
					}

					if (status.MPI_SOURCE == right_server_id() && !has_leader()) {
						set_server_asleep(0);
						MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), REPLY, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, right_server_id(), REPLY, MPI_COMM_WORLD);
					}
				}

				break;

			case REPLY:
				MPI_Recv(&rcv_msg.val_1, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				j = rcv_msg.pid;
				k = rcv_msg.val_1;
				
				if (status.MPI_SOURCE == left_server_id())
					set_server_l_reply();
				else 
					set_server_r_reply();

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
					send_msg = prepare_msg(rank, k + 1, 1, 0, 0);

					// Phase k winner
					if (is_server_l_reply() && is_server_r_reply()) {
						MPI_Send(&send_msg.pid, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_2, 1, MPI_INT, left_server_id(), PROB, MPI_COMM_WORLD);

						MPI_Send(&send_msg.pid, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_1, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
						MPI_Send(&send_msg.val_2, 1, MPI_INT, right_server_id(), PROB, MPI_COMM_WORLD);
					}
				}
				if (status.MPI_SOURCE == left_server_id())
					set_server_l_reply();
				else 
					set_server_r_reply();
				break;


			case LEADER:
				set_leader(rcv_msg.pid);

				if (rank == rcv_msg.pid)
					MPI_Send(&rcv_msg.pid, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD);
				else {
					MPI_Send(&rcv_msg.pid, 1, MPI_INT, right_server_id(), LEADER, MPI_COMM_WORLD);
				}

				break;

			case EXIT:
				return;
				break;
		}
	}
}
