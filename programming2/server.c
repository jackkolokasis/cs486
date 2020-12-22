#include "server.h"

void server_init(int id, int l_id, int r_id) {
	server.s_rank = id;
	server.l_rank = l_id;
	server.r_rank = r_id;
}
