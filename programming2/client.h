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
	struct _neighbor *child;	// List of childs  
	struct _neighbor *parent;	// List of parents
	struct _neighbor *other;	// List of parents

	int purchase;
};

struct _client client;


void new_client(int id);

int client_alive();

void print_client();

struct _neighbor* internal_insert_nbr(struct _neighbor *head, int id);

void insert_nbr_client(int id);

struct _neighbor* get_client_nbrs();

void add_client_parent(int id);

int has_client_parent();

void add_client_child(int id);

void add_client_other(int id);

int contains_client_nbrs();






#endif
