#ifndef __SERVER_H__
#define __SERVER_H__

struct _server {
	int s_rank;
	int l_rank;
	int r_rank;

	int asleep;
	int stock;
	int active_requests;
};

struct _server server;

void server_init(int id, int l_id, int r_id);

int server_id();

int left_server_id();

int right_server_id();

#endif
