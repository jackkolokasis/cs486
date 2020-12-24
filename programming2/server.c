#include "server.h"
#include <stdio.h>

void init_server(int id, int l_id, int r_id) {
	server.s_rank = id;
	server.l_rank = l_id;
	server.r_rank = r_id;

	server.asleep = 1;

	server.l_reply = 0;
	server.r_reply = 0;
	server.l_found = 0;
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

void set_server_l_reply() {
	server.l_reply = 1;
}

void set_server_r_reply() {
	server.r_reply = 1;
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


