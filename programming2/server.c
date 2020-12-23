#include "server.h"

void server_init(int id, int l_id, int r_id) {
	server.s_rank = id;
	server.l_rank = l_id;
	server.r_rank = r_id;

	server.asleep = 1;
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
