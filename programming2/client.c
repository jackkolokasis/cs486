#include <stdlib.h>
#include "client.h"
#include <stdio.h>

static int exist = 0;

// Create new client with `id`
void new_client(int id) {
	exist = 1;

	client.id = id;
	client.num_child = 0;

	client.nbr = NULL;
	client.child = NULL;
	client.parent = NULL;
	client.other = NULL;
}

int client_alive() { return exist; }

// Insert a new neighbor client with `id` in the list of neighbors
void insert_nbr_client(int id) {
	client.nbr = internal_insert_nbr(client.nbr, id);
}

struct _neighbor* internal_insert_nbr(struct _neighbor *head, int id) {

	struct _neighbor *tmp = NULL;

	tmp = malloc(sizeof(struct _neighbor));
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

void print_client() {
	struct _neighbor *tmp;

	printf("==========================================\n");
	printf(" CLIENT ID: %d\n", client.id);
	printf(" NEIGHBORS: ");
	for (tmp = client.nbr; tmp != NULL; tmp = tmp->next)
		printf("%d ", tmp->id);
	
	printf("\n PARENTS: ");
	for (tmp = client.parent; tmp != NULL; tmp = tmp->next)
		printf("%d ", tmp->id);

	printf("\n CHILDREN: ");
	for (tmp = client.child; tmp != NULL; tmp = tmp->next)
		printf("%d ", tmp->id);
	
	printf("\n OTHER: ");
	for (tmp = client.other; tmp != NULL; tmp = tmp->next)
		printf("%d ", tmp->id);

	printf("\n==========================================\n");
}

void add_client_child(int id) {
	client.child = internal_insert_nbr(client.child, id);
	client.num_child++;
}

void add_client_parent(int id) {
	client.parent = internal_insert_nbr(client.parent, id);
}

int has_client_parent() {
	return client.parent != NULL;
}

struct _neighbor* get_client_nbrs() {
	return client.nbr;
}

void add_client_other(int id) {
	client.other = internal_insert_nbr(client.other, id);
}

int contains_client_nbrs() {
	int sum_nbrs = 0;
	int sum_child = 0;
	int sum_other = 0;
	int sum_par = 0;
	
	struct _neighbor *tmp;

	for (tmp = client.nbr; tmp != NULL; tmp = tmp->next)
		sum_nbrs += tmp->id;

	for (tmp = client.child; tmp != NULL; tmp = tmp->next)
		sum_child += tmp->id;

	for (tmp = client.other; tmp != NULL; tmp = tmp->next)
		sum_other += tmp->id;
	
	for (tmp = client.parent; tmp != NULL; tmp = tmp->next)
		sum_par += tmp->id;
	
	return (sum_nbrs - sum_par == sum_child + sum_other);
}

int has_client_children() {
	return client.child != NULL;
}

struct _neighbor* get_client_parent() {
	return client.parent;

}

struct _neighbor* get_client_children() {
	return client.child;
}

int num_client_child() {
	return client.num_child;
}
