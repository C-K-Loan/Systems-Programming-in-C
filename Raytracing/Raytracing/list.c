#include "list.h"

#include <stdlib.h>

list_t* list_create() {
	list_t *list = (list_t*) malloc(sizeof(list_t));
	list->root = NULL;

	return list;
}

void list_add(list_t *list, void *data, enum node_type type) {
	node_t *node = (node_t*) malloc(sizeof(node_t));
	node->next = NULL;
	node->data = data;
	node->type = type;

	if (list->root == NULL) {
		list->root = node;
	} else {
		node_t *curr = list->root;
		while (curr->next != NULL) {
			curr = curr->next;
		}
		curr->next = node;
	}
}

