#ifndef LIST_H_
#define LIST_H_

enum node_type {
	SPHERE, MESH
};

typedef struct node_t {
	struct node_t *next;
	void *data;
	enum node_type type;
} node_t;

typedef struct {
	node_t *root;
} list_t;

list_t* list_create();

void list_add(list_t *list, void *data, enum node_type);

#endif
