#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdlib.h>

struct _neighbor {
	int id;						// Id of the neighbor client
	struct _neighbor *next;		// Next node in the neighbor clients list
};

struct _client {
	int id;						// Id of the client
	struct _neighbor *nbr;		// Neighbors of this client

	int purchase;
};

struct _client client;

void new_client(int id);

int client_alive();

void insert_nbr_client(int id);

struct _neighbor* internal_insert_nbr(struct _neighbor *head, int id);



#endif
