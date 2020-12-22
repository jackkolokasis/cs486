#ifndef __SERVER_H__
#define __SERVER_H__

struct _server {
	int s_rank;
	int l_rank;
	int r_rank;

	int stock;
	int active_requests;
};

struct _server server;

void server_init(int id, int l_id, int r_id);

#endif
