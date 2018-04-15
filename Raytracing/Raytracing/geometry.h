#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <stdint.h>

#include "math.h"
#include "scene.h"

typedef struct {
	vec_t p;
	vec_t n;
} vert_t;

typedef struct {
	int a;
	int b;
	int c;
	vec_t n;
	mat_t *mat;
	struct mesh_t *mesh;
} tri_t;

typedef struct {
	vec_t pos;
	float radius;
	mat_t *mat;
	float size;
} sphere_t;

typedef struct mesh_t {
	vert_t *verts;
	tri_t *tris;
	int num_verts;
	int num_tris;
	float size;
} mesh_t;

typedef struct {
	vec_t origin;
	vec_t direction;
	uint8_t recursion;
} ray_t;

typedef struct {
	vec_t pos;
	vec_t n;
	ray_t *ray;
	mat_t *mat;
	sphere_t *sphere;
	mesh_t * mesh;
	enum node_type type;
	float u;
	float v;
} intersection_t;

void tri_create_normal(tri_t *t);

void mesh_invert_normals(mesh_t *m);

mesh_t* create_cube(vec_t min, vec_t max, mat_t **mat);

mesh_t* create_icosasphere(vec_t pos, float radius, mat_t *mat, int subdiv);

sphere_t* create_sphere(vec_t pos, float radius, mat_t *mat);

void mesh_scale(mesh_t *mesh, float scale);

void mesh_translate(mesh_t *mesh, float x, float y, float z);

void mesh_rotate_x(mesh_t *mesh, float angle);

void mesh_rotate_y(mesh_t *mesh, float angle);

void mesh_rotate_z(mesh_t *mesh, float angle);

void print_vec(vec_t *vec);

void print_tri(tri_t *tri);

#endif
