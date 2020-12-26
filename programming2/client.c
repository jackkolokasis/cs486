#include <stdlib.h>
#include "client.h"
#include <stdio.h>

static int exist = 0;

// Create new client with `id`
void new_client(int id) {
	exist = 1;
	client.id = id;
	client.nbr = NULL;
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

	for (tmp = client.nbr; tmp != NULL; tmp = tmp->next) {
		printf("%d ", tmp->id);
	}

	printf("\n==========================================\n");
}
