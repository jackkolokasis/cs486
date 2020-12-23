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

	case EXIT:
		MPI_Send(&msg->pid, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;
	}
}

void my_receive(struct _msg *msg) {
	MPI_Status status;				// Tag status
	struct _msg rcv_msg;			// Receive message
	struct _msg ack_msg;			// Ack message

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

			case EXIT:
				return;
				break;
		}
	}
}
