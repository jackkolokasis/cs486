#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include "sharedDefines.h"

void init_server(int id, int l_id, int r_id) {
	server.s_rank = id;
	server.l_rank = l_id;
	server.r_rank = r_id;

	server.leader = -1;
	server.is_leader = 0;
	server.asleep = 1;

	server.l_reply = 0;
	server.r_reply = 0;
	server.l_found = 0;
	
	server.lead_l_reply = 0;
	server.lead_r_reply = 0;
	
	server.num_child = 0;

	server.nbr = NULL;
	server.child = NULL;
	server.parent = NULL;
	server.other = NULL;

	server.stock = 1000;
	server.ext_supply = 0;
}

int server_id() {
	return server.s_rank;
}

int left_server_id() {
	return server.l_rank;
}

int right_server_id() {
	return server.r_rank;
}

int is_server_asleep() {
	return server.asleep;
}

void set_server_asleep(int val) {
	server.asleep = val;
}

void set_server_l_reply(int val) {
	server.l_reply = val;
}

void set_server_r_reply(int val) {
	server.r_reply = val;
}

int is_server_l_reply() {
	return server.l_reply;
}

int is_server_r_reply() {
	return server.l_reply;
}

void set_leader(int leader) {
	server.l_found = 1;
	server.leader = leader;
}

int get_leader() {
	return server.leader; 
}

int has_leader() {
	return server.l_found;
}

void make_leader() {
	server.is_leader = 1;
}

int is_leader() {
	return server.is_leader;
}

void add_server_child(int id) {
	server.child = internal_insert_child(server.child, id);
	server.num_child++;
}

void add_server_other(int id) {
	server.other = internal_insert_child(server.other, id);
}

struct _child* internal_insert_child(struct _child *head, int id) {
	struct _child *tmp = NULL;

	tmp = malloc(sizeof(struct _child));
	tmp->id = id;
	tmp->next = NULL;

	if (head == NULL) {
		head = tmp;
	}
	else {
		tmp->next = head;
		head = tmp;
	}

	return head;
}

void print_server() {
	struct _child *tmp;

	DPRINT("==========================================\n");
	DPRINT(" SERVER ID: %d\n", server.s_rank);
	DPRINT(" LEFT SERVER ID: %d\n", server.l_rank);
	DPRINT(" RIGHT SERVER ID: %d\n", server.r_rank);
	DPRINT(" LEADER ID: %d\n", server.leader);

	DPRINT(" NEIGHBORS: ");
	for (tmp = server.nbr; tmp != NULL; tmp = tmp->next)
		DPRINT("%d ", tmp->id);

	DPRINT("\n CHILDREN: ");
	for (tmp = server.child; tmp != NULL; tmp = tmp->next)
		DPRINT("%d ", tmp->id);
	
	DPRINT("\n OTHER: ");
	for (tmp = server.other; tmp != NULL; tmp = tmp->next)
		DPRINT("%d ", tmp->id);
	
	DPRINT("\n STOCK: %d\n", server.stock);

	DPRINT("\n==========================================\n");
}

void set_server_leader_l_reply(int val) {
	server.lead_l_reply = val;
}

void set_server_leader_r_reply(int val) {
	server.lead_r_reply = val;
}

int is_server_leader_l_reply() {
	return server.lead_l_reply;
}

int is_server_leader_r_reply() {
	return server.lead_r_reply;
}

struct _child* get_server_nbrs() {
	return server.nbr;
}

// Insert a new neighbor client with `id` in the list of neighbors of the server
void insert_nbr_server(int id) {
	server.nbr = internal_insert_child(server.nbr, id);
}

int contains_server_nbrs() {
	int sum_nbrs = 0;
	int sum_child = 0;
	int sum_other = 0;
	
	struct _child *tmp;

	for (tmp = server.nbr; tmp != NULL; tmp = tmp->next)
		sum_nbrs += tmp->id;

	for (tmp = server.child; tmp != NULL; tmp = tmp->next)
		sum_child += tmp->id;
	
	for (tmp = server.other; tmp != NULL; tmp = tmp->next)
		sum_other += tmp->id;

	return (sum_nbrs == sum_child + sum_other);
}

int has_children() {
	return server.child != NULL;
}

struct _child* get_server_children() {
	return server.child;
}

int num_server_child() {
	return server.num_child;
}

void incr_stock(int val) {
	server.stock += val;
}

void decr_stock(int val) {
	server.stock -= val;

}

int get_stock() {
	return server.stock;

}

void set_stock(int val) {
	server.stock = val;
}

void set_ext_supply(int val) {
	server.ext_supply = val;
}

int get_ext_supply() {
	return server.ext_supply;
}
