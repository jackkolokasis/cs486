#ifndef __SERVER_H__
#define __SERVER_H__

struct _child {
	int id;						// Id of the neighbor client
	struct _child *next;		// Next node in the neighbor clients list
};

struct _server {
	int s_rank;
	int l_rank;
	int r_rank;
	int leader;
	int is_leader;

	int asleep;
	
	int l_reply;
	int r_reply;
	int l_found;

	int lead_l_reply;
	int lead_r_reply;

	int stock;
	int active_requests;

	struct _child *child;

	
};

struct _server server;

void init_server(int id, int l_id, int r_id);

int server_id();

int left_server_id();

int right_server_id();

int is_server_asleep();

void set_server_asleep();

void set_server_l_reply(int val);

void set_server_r_reply(int val);

int is_server_l_reply();

int is_server_r_reply();

void set_leader(int leader);

int has_leader();

int get_leader();

void make_leader();

int is_leader();

void add_server_child(int id);

struct _child* internal_insert_child(struct _child *head, int id);

void print_server();


void set_server_leader_l_reply(int val);

void set_server_leader_r_reply(int val);

int is_server_leader_l_reply();

int is_server_leader_r_reply();




#endif
