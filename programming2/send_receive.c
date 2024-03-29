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

	case SPANNING_TREE:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;

	case COUNT:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;
	
	case ORDER:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;
	
	case SUPPLY:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;
	
	case EXTERNAL_SUPPLY:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;
	
	case PRINT:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;
	
	case REPORT:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;

	case EXIT:
		MPI_Send(msg, MSG_SIZE, MPI_INT, rank, tag, MPI_COMM_WORLD);
		break;
	}
}

// Process `rank` receive message `msg`. Based on the tag value perform the
// following actions
void my_receive(int *msg, int rank, int num_servers) {
	MPI_Status status;				// Tag status

	int rcv_msg[MSG_SIZE];			// Receive message
	int send_msg[MSG_SIZE];			// Send message
	int send_msg_l[MSG_SIZE];		// Send message to the left server
	int send_msg_r[MSG_SIZE];		// Send message to the right server
	int ack_msg[MSG_SIZE];			// Ack message

	int jl, kl, dl;				    // Id, Phase, Distance for left server
	int jr, kr, dr;				    // Id, Phase, Distance for left server
	int client1, client2;			// Clients
	int p, i;

	struct _child *s_tmp;			// Server children
	struct _neighbor *c_tmp;		// Client neighbor

	int num_reply = 0;					// Number of replies by children
	int num_vertex = 0;					// Number of vertex
	int total_num_vertex = 0;			// Total number of vertex from all servers
	int count = 0;						// Counter
	int quantity;					// Quantity and diff to supply
	int purchases = 0;					// Number of purchases for each client childrens
	int total_purchases = 0;			// Total purchases from all client in a server tree
	int replies = 0;					// Number of replies

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
					set_server_leader_l_reply(1);

					if (is_server_leader_r_reply()) {
						// Prepare msg
						prepare_msg(send_msg, rank, 0, 0, 0 , 0, 0);
						set_leader(rank);
						make_leader();
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, right_server_id(), LEADER, MPI_COMM_WORLD);
					}
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
					set_server_leader_r_reply(1);

					if (is_server_leader_l_reply()) {
						// Prepare msg
						prepare_msg(send_msg, rank, 0, 0, 0 ,0, 0);
						set_leader(rank);
						make_leader();
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, right_server_id(), LEADER, MPI_COMM_WORLD);
					}
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
				if (is_leader()) {
					prepare_msg(send_msg, rcv_msg[0], 0, 0, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);
				}
				else {
					set_leader(rcv_msg[0]);
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

				if (client2 <= num_servers) {
					insert_nbr_server(client1);
				}
				else {

					if (!client_alive())
						// Create new client - client1
						new_client(client2);

					// Add client1 to the list of neighbors of client2
					insert_nbr_client(client1);
				}

				// Prepare ack message
				prepare_msg(send_msg, client1, client2, 0, 0, 0, 0); 
				MPI_Send(send_msg, MSG_SIZE, MPI_INT, client1, ACK, MPI_COMM_WORLD);

				break;

			case SPANNING_TREE:
				DPRINT("SPANNING_TREE\n");
				for (i = 1; i <= num_servers; i++) {
					// Prepare message
					prepare_msg(send_msg, i, 0, 0, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, i, S_TREE, MPI_COMM_WORLD);
				}
				break;

			case S_TREE:
                if (get_server_nbrs() == NULL) {
                    prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
                    MPI_Send(send_msg, MSG_SIZE, MPI_INT, get_leader(), TOTAL, MPI_COMM_WORLD);
                    MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, TERMINATE, MPI_COMM_WORLD);
                    break;
                }

				for (s_tmp = get_server_nbrs(); s_tmp != NULL; s_tmp = s_tmp->next) {
					DPRINT("S_TREE\n");
					prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, s_tmp->id, MSG, MPI_COMM_WORLD);
				}

				break;

			case MSG:
				DPRINT("MSG\n");
				p = rcv_msg[0];

				// Prepare msg
				prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);

				if (!has_client_parent()) {
					add_client_parent(p);

					MPI_Send(send_msg, MSG_SIZE, MPI_INT, p, PARENT, MPI_COMM_WORLD);

					for (c_tmp = get_client_nbrs(); c_tmp != NULL; c_tmp = c_tmp->next)
						if (c_tmp->id != p)
							MPI_Send(send_msg, MSG_SIZE, MPI_INT, c_tmp->id, MSG, MPI_COMM_WORLD);
				}
				else {
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, p, ALREADY, MPI_COMM_WORLD);
				}
				break;

			case PARENT:
				p = rcv_msg[0];

				if (rank <= num_servers) {
					add_server_child(p);
					if (contains_server_nbrs()) { // Terminate
						// Prepare message
						prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
						MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, TERMINATE, MPI_COMM_WORLD);
						break;
					}
				}
				else {
					add_client_child(p);
					if (contains_client_nbrs()) { // Terminate
						// Prepare message
						//prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
						//MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, TERMINATE, MPI_COMM_WORLD);
						break;
					}
				}

				break;

			case ALREADY:
				p = rcv_msg[0];

				if (rank <= num_servers) {
					add_server_other(p);

					if (contains_server_nbrs()) { // Terminate
						// Prepare message
						prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
						MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, TERMINATE, MPI_COMM_WORLD);
					}
				}
				else {

					add_client_other(p);

					if (contains_client_nbrs()) { // Terminate
						// Prepare message
						//prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
						//MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, TERMINATE, MPI_COMM_WORLD);
						break;
					}
				}
				break;

			case COUNT:
				p = rcv_msg[0];

				if (rank <= num_servers) {
					prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);
					for (s_tmp = get_server_children(); s_tmp != NULL; s_tmp = s_tmp->next) {
						DPRINT("COUNT | ID = %d\n", s_tmp->id);
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, s_tmp->id, COUNT, MPI_COMM_WORLD);
					}
				}
				else {
					if (has_client_children()) {
						prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);

						for (c_tmp = get_client_children(); c_tmp != NULL; c_tmp = c_tmp->next)
							MPI_Send(send_msg, MSG_SIZE, MPI_INT, c_tmp->id, COUNT, MPI_COMM_WORLD);
					}
					else {
						prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, p, COLLECT, MPI_COMM_WORLD);
					}
				}

				break;

			case COLLECT:
				p = rcv_msg[0];
				num_vertex += rcv_msg[1];

				// Increase the number of replies
				num_reply++;

				if (rank <= num_servers) {
					if (num_reply == num_server_child()) {
						num_vertex += num_server_child();
						prepare_msg(send_msg, rank, num_vertex, 0, 0, 0, 0);
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, get_leader(), TOTAL, MPI_COMM_WORLD);
					}
				}
				else {
					if (num_reply == num_client_child()) {
						num_vertex += num_client_child();
						prepare_msg(send_msg, rank, num_vertex, 0, 0, 0, 0);
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, get_client_parent()->id, COLLECT, MPI_COMM_WORLD);
					}
				}
				break;

			case TOTAL:
				count ++;
				total_num_vertex += rcv_msg[1];

				printf("SPANNING_TREE: S_ID: %d | NODES: %d\n", rcv_msg[0], rcv_msg[1]);
				fflush(stdout);

				if (count == num_servers) {
					printf("REPORT: S_ID: %d | NODES: %d\n", rank, total_num_vertex);
					fflush(stdout);

					total_num_vertex = 0;
					count = 0;

					prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
					MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);
				}

				break;
	
			case ORDER:
				incr_purchase(rcv_msg[1]); // Increase purcases by num

				prepare_msg(send_msg, rcv_msg[0], rcv_msg[1], 0, 0, 0, 0);
				MPI_Send(send_msg, MSG_SIZE, MPI_INT, get_client_parent()->id, P_ORDER, MPI_COMM_WORLD);

				break;

			case P_ORDER:
				if (rank <= num_servers) {
					int client = rcv_msg[0];
					int val = rcv_msg[1];
					
					decr_stock(val);

					prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
					MPI_Send(ack_msg, MSG_SIZE, MPI_INT, client, ACK, MPI_COMM_WORLD);
				}
				else {
					prepare_msg(send_msg, rcv_msg[0], rcv_msg[1], 0, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, get_client_parent()->id, P_ORDER, MPI_COMM_WORLD);
				}
				break;

			case SUPPLY:
				quantity = rcv_msg[1];

				if (rank == rcv_msg[0] && status.MPI_SOURCE == 0) {
					prepare_msg(send_msg, rcv_msg[0], quantity, 0, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, left_server_id(), SUPPLY, MPI_COMM_WORLD);
					break;
				}

				if (rank == rcv_msg[0] && status.MPI_SOURCE == right_server_id()) {
					// Current amount received from other servers
					int curr_rcv_amount = rcv_msg[2];

					int receive = get_stock() - 150;
					set_stock(150);

					quantity -= receive;
					curr_rcv_amount += receive;
					
					prepare_msg(ack_msg, rcv_msg[0], quantity, curr_rcv_amount, 0, 0, 0);
					MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, SUPPLY_ACK, MPI_COMM_WORLD);
					break;
				}

				if (get_stock() > (150 + quantity)) {
					// Current amount received from other servers
					int curr_rcv_amount = rcv_msg[2];

					decr_stock(quantity);
					
					curr_rcv_amount += quantity;

					prepare_msg(ack_msg, rcv_msg[0], 0, curr_rcv_amount, 0, 0, 0);
					MPI_Send(ack_msg, MSG_SIZE, MPI_INT, rcv_msg[0], GET_SUPPLY, MPI_COMM_WORLD);
				} 
				else if (get_stock() > 150 && get_stock() < (150 + quantity)) {
					// Current amount received from other servers
					int curr_rcv_amount = rcv_msg[2];

					int receive = get_stock() - 150;
					set_stock(150);

					quantity -= receive;
					curr_rcv_amount += receive;

					prepare_msg(send_msg, rcv_msg[0], quantity, curr_rcv_amount, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, left_server_id(), SUPPLY, MPI_COMM_WORLD);
				}
				else if (get_stock() <= 150) {
					prepare_msg(send_msg, rcv_msg[0], rcv_msg[1], rcv_msg[2], 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, left_server_id(), SUPPLY, MPI_COMM_WORLD);
				}

				break;

			case GET_SUPPLY:
				incr_stock(rcv_msg[2]);
				prepare_msg(ack_msg, rcv_msg[0], rcv_msg[1], rcv_msg[2], 0, 0, 0);
				MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, SUPPLY_ACK, MPI_COMM_WORLD);
				break;
	
			case EXTERNAL_SUPPLY:
				set_ext_supply(rcv_msg[0]);

				if (get_stock() < 150) {
					int req = 150 - get_stock();

					if (req >= get_ext_supply()) {
						set_stock(get_ext_supply());
						set_ext_supply(0);

						prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
						MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);
						break;
					}
					else {
						set_stock(req);
						set_ext_supply(get_ext_supply() - req);
					}
				}

				prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);
				MPI_Send(send_msg, MSG_SIZE, MPI_INT, left_server_id(), CHECK_SUPPLY, MPI_COMM_WORLD);

				break;

			case CHECK_SUPPLY:
				if (rank == get_leader()) {
					incr_stock(get_ext_supply());
					set_ext_supply(0);

					prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
					MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);
					break;
				}

				if (get_stock() < 150) {
					int req = 150 - get_stock();
					prepare_msg(send_msg, rank, req, 0, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, get_leader(), SUPPLY_REQ, MPI_COMM_WORLD);
				}
				else {
					prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, left_server_id(), CHECK_SUPPLY, MPI_COMM_WORLD);
				}

				break;

			case SUPPLY_REQ:
				if (rcv_msg[1] >= get_ext_supply()) {
					int req = get_ext_supply();
					set_ext_supply(0);

					prepare_msg(send_msg, req, 0, 0, 0, 0, 0);
				}
				else {
					int req = rcv_msg[1];
					set_ext_supply(get_ext_supply() - req);

					prepare_msg(send_msg, req, 0, 0, 0, 0, 0);
				}
					
				MPI_Send(send_msg, MSG_SIZE, MPI_INT, rcv_msg[0], ACK_SUPPLY_REQ, MPI_COMM_WORLD);
				break;

			case ACK_SUPPLY_REQ:
				incr_stock(rcv_msg[0]);

				prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);
				MPI_Send(send_msg, MSG_SIZE, MPI_INT, left_server_id(), CHECK_SUPPLY, MPI_COMM_WORLD);

				break;

			case PRINT:
				if (rank == get_leader() && status.MPI_SOURCE == right_server_id()) {
					prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
					MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);
				}
				else {
					printf("SERVER %d HAS QUANTITY %d\n", rank, get_stock());
					fflush(stdout);
					
					prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);
					MPI_Send(send_msg, MSG_SIZE, MPI_INT, left_server_id(), PRINT, MPI_COMM_WORLD);
				}

				break;
			
			case REPORT:
				p = rcv_msg[0];

				if (rank <= num_servers) {
					prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);
                    if (get_server_children() == NULL) {
                        MPI_Send(send_msg, MSG_SIZE, MPI_INT, get_leader(), TOTAL_REPORT, MPI_COMM_WORLD);
                        break;
                    }
					for (s_tmp = get_server_children(); s_tmp != NULL; s_tmp = s_tmp->next) {
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, s_tmp->id, REPORT, MPI_COMM_WORLD);
					}
				}
				else {
					if (has_client_children()) {
						prepare_msg(send_msg, rank, 0, 0, 0, 0, 0);

						for (c_tmp = get_client_children(); c_tmp != NULL; c_tmp = c_tmp->next)
							MPI_Send(send_msg, MSG_SIZE, MPI_INT, c_tmp->id, REPORT, MPI_COMM_WORLD);
					}
					else {
						prepare_msg(send_msg, rank, get_purchase(), 0, 0, 0, 0);
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, p, REP_COLLECT, MPI_COMM_WORLD);
					}
				}

				break;

			case REP_COLLECT:
				p = rcv_msg[0];
				purchases += rcv_msg[1];

				// Increase the number of replies
				replies++;

				if (rank <= num_servers) {
					if (replies == num_server_child()) {
                        replies = 0;
						prepare_msg(send_msg, rank, purchases, 0, 0, 0, 0);
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, get_leader(), TOTAL_REPORT, MPI_COMM_WORLD);
					}
				}
				else {
					if (replies == num_client_child()) {
                        replies = 0;
						purchases += get_purchase();
						prepare_msg(send_msg, rank, purchases, 0, 0, 0, 0);
						MPI_Send(send_msg, MSG_SIZE, MPI_INT, get_client_parent()->id, REP_COLLECT, MPI_COMM_WORLD);
					}
				}
				break;
			
			case TOTAL_REPORT:
				count ++;
				DPRINT("COUNT = %d", count);
				total_purchases += rcv_msg[1];

				printf("REPORT S_ID: %d | QUANTITY: %d\n", rcv_msg[0], rcv_msg[1]);
				fflush(stdout);

				if (count == num_servers) {
					printf("TOTAL REPORT S_ID: %d | QUANTITY: %d\n", rank, total_purchases);
					fflush(stdout);

					total_purchases = 0;
					count = 0;

					prepare_msg(ack_msg, rank, 0, 0, 0, 0, 0);
					MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);
				}

				break;
			
			case ACK:
				DPRINT("[ACK] %d -> %d\n", rcv_msg[0], rcv_msg[1]);
				
				// Send ack message to client 1
				prepare_msg(ack_msg, rcv_msg[0], rcv_msg[1], 0, 0, 0, 0);
				MPI_Send(ack_msg, MSG_SIZE, MPI_INT, 0, ACK, MPI_COMM_WORLD);

				break;

			case EXIT:
				if (rank <= num_servers)
					print_server();
				else if (rank > num_servers)
					print_client();

				return;
				break;
		}
	}
}
