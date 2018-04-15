#include "geometry.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void tri_create_normal(tri_t *t) {
	vert_t *verts = t->mesh->verts;

	vec_t e1 = vec_sub(&(verts[t->a].p), &(verts[t->b].p));
	vec_t e2 = vec_sub(&(verts[t->b].p), &(verts[t->c].p));
	vec_t cross = vec_cross(&e1, &e2);

	t->n = vec_normalize(&cross);
}

void mesh_invert_normals(mesh_t *m) {
	int i;
	for (i = 0; i < m->num_tris; i++) {
		tri_t *t = m->tris + i;

		t->n = vec_multiply(&(t->n), -1.0);
	}
}

mesh_t* create_cube(vec_t min, vec_t max, mat_t **mat) {
	tri_t *tris = (tri_t*) calloc(12, sizeof(tri_t));
	vert_t *verts = (vert_t*) calloc(8, sizeof(vert_t));
	mesh_t *mesh = (mesh_t*) malloc(sizeof(mesh_t));

	vec_t n_tmp = { 0, 0, 0 };

	verts[0] = (vert_t ) { (vec_t ) { min.x, min.y, min.z }, n_tmp };
	verts[1] = (vert_t ) { (vec_t ) { min.x, min.y, max.z }, n_tmp };
	verts[2] = (vert_t ) { (vec_t ) { min.x, max.y, min.z }, n_tmp };
	verts[3] = (vert_t ) { (vec_t ) { min.x, max.y, max.z }, n_tmp };
	verts[4] = (vert_t ) { (vec_t ) { max.x, min.y, min.z }, n_tmp };
	verts[5] = (vert_t ) { (vec_t ) { max.x, min.y, max.z }, n_tmp };
	verts[6] = (vert_t ) { (vec_t ) { max.x, max.y, min.z }, n_tmp };
	verts[7] = (vert_t ) { (vec_t ) { max.x, max.y, max.z }, n_tmp };

	tris[0] = (tri_t ) { 0, 1, 2, n_tmp, mat[0], mesh };
	tris[1] = (tri_t ) { 3, 2, 1, n_tmp, mat[0], mesh };

	tris[2] = (tri_t ) { 6, 5, 4, n_tmp, mat[1], mesh };
	tris[3] = (tri_t ) { 5, 6, 7, n_tmp, mat[1], mesh };

	tris[4] = (tri_t ) { 0, 4, 5, n_tmp, mat[2], mesh };
	tris[5] = (tri_t ) { 5, 1, 0, n_tmp, mat[2], mesh };

	tris[6] = (tri_t ) { 6, 4, 0, n_tmp, mat[3], mesh };
	tris[7] = (tri_t ) { 0, 2, 6, n_tmp, mat[3], mesh };

	tris[8] = (tri_t ) { 5, 3, 1, n_tmp, mat[4], mesh };
	tris[9] = (tri_t ) { 3, 5, 7, n_tmp, mat[4], mesh };

	tris[10] = (tri_t ) { 7, 6, 3, n_tmp, mat[5], mesh };
	tris[11] = (tri_t ) { 2, 3, 6, n_tmp, mat[5], mesh };

	mesh->tris = tris;
	mesh->verts = verts;
	mesh->num_tris = 12;
	mesh->num_verts = 8;

	vec_t tmp = vec_sub(&max, &min);
	mesh->size = vec_len(&tmp);

	int i;
	for (i = 0; i < 12; i++) {
		tri_create_normal(tris + i);
	}
	return mesh;
}

mesh_t* create_icosasphere(vec_t pos, float radius, mat_t *mat, int subdiv) {
	// Icosasphere has 20-sides, 12-vertices. Vertices are described by:
	// (+-1, 0, +-t), (0, +-t, +-1), and (+-t, +-1, 0)
	// where t = (sqrt(5)-1)/2, the golden ratio.

	int num_tris = 20;
	int num_verts = 12;

	float t = (sqrt(5.0) - 1.0) / 2.0;

	vec_t p[num_verts];
	vec_t n[num_verts];

	vert_t *verts = (vert_t*) calloc(num_verts, sizeof(vert_t));
	tri_t *tris = (tri_t*) calloc(num_tris, sizeof(tri_t));
	mesh_t *mesh = (mesh_t*) malloc(sizeof(mesh_t));

	int idx[] = { 0, 11, 3, 0, 3, 8, 0, 5, 11, 0, 4, 5, 1, 9, 2, 1, 2, 10, 1,
			10, 5, 1, 5, 4, 5, 10, 11, 6, 11, 10, 10, 2, 6, 4, 8, 9, 8, 7, 9, 2,
			9, 7, 2, 7, 6, 3, 11, 6, 0, 8, 4, 3, 7, 8, 3, 6, 7, 1, 4, 9 };

	n[0] = (vec_t ) { 1, 0, t };
	n[1] = (vec_t ) { -1, 0, t };
	n[2] = (vec_t ) { -1, 0, -t };
	n[3] = (vec_t ) { 1, 0, -t };

	n[4] = (vec_t ) { 0, t, 1 };
	n[5] = (vec_t ) { 0, -t, 1 };
	n[6] = (vec_t ) { 0, -t, -1 };
	n[7] = (vec_t ) { 0, t, -1 };

	n[8] = (vec_t ) { t, 1, 0 };
	n[9] = (vec_t ) { -t, 1, 0 };
	n[10] = (vec_t ) { -t, -1, 0 };
	n[11] = (vec_t ) { t, -1, 0 };

	int i;

	for (i = 0; i < num_verts; i++) {
		n[i] = vec_normalize(&n[i]);
		p[i] = vec_multiply(&n[i], radius);
		p[i] = vec_add(p + i, &pos);
		verts[i] = (vert_t ) { p[i], n[i] };
	}

	vec_t n_tmp = { 0, 0, 0 };

	for (i = 0; i < num_tris; i++) {
		tris[i] = (tri_t ) { idx[i * 3 + 0], idx[i * 3 + 1], idx[i * 3 + 2],
						n_tmp, mat, mesh };
	}

	mesh->tris = tris;
	mesh->verts = verts;
	mesh->num_tris = num_tris;
	mesh->num_verts = num_verts;

	for (i = 0; i < subdiv; i++) {
		verts = (vert_t*) realloc(verts,
				sizeof(vert_t) * (num_verts + num_tris * 3));

		tri_t *tri_tmp = (tri_t*) calloc(num_tris * 4, sizeof(tri_t));

		int j;
		for (j = 0; j < num_tris; j++) {
			vert_t *a = verts + tris[j].a;
			vert_t *b = verts + tris[j].b;
			vert_t *c = verts + tris[j].c;

			vec_t abn = (vec_t ) { a->n.x + (b->n.x - a->n.x) / 2.0, a->n.y
							+ (b->n.y - a->n.y) / 2.0, a->n.z
							+ (b->n.z - a->n.z) / 2.0 };
			vec_t bcn = (vec_t ) { b->n.x + (c->n.x - b->n.x) / 2.0, b->n.y
							+ (c->n.y - b->n.y) / 2.0, b->n.z
							+ (c->n.z - b->n.z) / 2.0 };
			vec_t can = (vec_t ) { c->n.x + (a->n.x - c->n.x) / 2.0, c->n.y
							+ (a->n.y - c->n.y) / 2.0, c->n.z
							+ (a->n.z - c->n.z) / 2.0 };

			abn = vec_normalize(&abn);
			bcn = vec_normalize(&bcn);
			can = vec_normalize(&can);

			vec_t abp = vec_multiply(&abn, radius);
			vec_t bcp = vec_multiply(&bcn, radius);
			vec_t cap = vec_multiply(&can, radius);

			abp = vec_add(&abp, &pos);
			bcp = vec_add(&bcp, &pos);
			cap = vec_add(&cap, &pos);

			int abi = num_verts + j * 3 + 0;
			int bci = num_verts + j * 3 + 1;
			int cai = num_verts + j * 3 + 2;

			verts[abi] = (vert_t ) { abp, abn };
			verts[bci] = (vert_t ) { bcp, bcn };
			verts[cai] = (vert_t ) { cap, can };

			tri_tmp[j * 4 + 0] = (tri_t ) { tris[j].a, abi, cai, n_tmp, mat,
							mesh };
			tri_tmp[j * 4 + 1] = (tri_t ) { abi, tris[j].b, bci, n_tmp, mat,
							mesh };
			tri_tmp[j * 4 + 2] = (tri_t ) { bci, tris[j].c, cai, n_tmp, mat,
							mesh };
			tri_tmp[j * 4 + 3] = (tri_t ) { cai, abi, bci, n_tmp, mat, mesh };
		}

		num_verts += num_tris * 3;
		num_tris *= 4;

		free(tris);
		tris = tri_tmp;

		mesh->tris = tris;
		mesh->verts = verts;
		mesh->num_tris = num_tris;
		mesh->num_verts = num_verts;
	}

	mesh->size = radius * 2.0;

	for (i = 0; i < num_tris; i++) {
		tri_create_normal(tris + i);
	}

	return mesh;
}

sphere_t* create_sphere(vec_t pos, float radius, mat_t *mat) {
	sphere_t *sphere = (sphere_t*) malloc(sizeof(sphere_t));
	sphere->radius = radius;
	sphere->pos = pos;
	sphere->mat = mat;
	sphere->size = radius * 2.0;
	return sphere;
}

void mesh_scale(mesh_t *mesh, float scale) {
	vert_t *verts = mesh->verts;
	int i;
	for (i = 0; i < mesh->num_verts; i++) {
		verts[i].p = vec_multiply(&(verts[i].p), scale);
	}
}

void mesh_translate(mesh_t *mesh, float x, float y, float z) {
	vert_t *verts = mesh->verts;
	vec_t p = { x, y, z };
	int i;
	for (i = 0; i < mesh->num_verts; i++) {
		verts[i].p = vec_add(&(verts[i].p), &p);
	}
}

void mesh_rotate_x(mesh_t *mesh, float angle) {
	vert_t *verts = mesh->verts;
	int i;
	for (i = 0; i < mesh->num_verts; i++) {
		verts[i].p = vec_rotate_x(&(verts[i].p), angle);
		verts[i].n = vec_rotate_x(&(verts[i].n), angle);
	}
}

void mesh_rotate_y(mesh_t *mesh, float angle) {
	vert_t *verts = mesh->verts;
	int i;
	for (i = 0; i < mesh->num_verts; i++) {
		verts[i].p = vec_rotate_y(&(verts[i].p), angle);
		verts[i].n = vec_rotate_y(&(verts[i].n), angle);
	}
}

void mesh_rotate_z(mesh_t *mesh, float angle) {
	vert_t *verts = mesh->verts;
	int i;
	for (i = 0; i < mesh->num_verts; i++) {
		verts[i].p = vec_rotate_z(&(verts[i].p), angle);
		verts[i].n = vec_rotate_z(&(verts[i].n), angle);
	}
}

void print_vec(vec_t *vec) {
	printf("[x=%f y=%f z=%f]\n", vec->x, vec->y, vec->z);
}

void print_tri(tri_t *tri) {
	printf("[a=%i b=%i c=%i n=[x=%f y=%f z=%f]]\n", tri->a, tri->b, tri->c,
			tri->n.x, tri->n.y, tri->n.z);
}
