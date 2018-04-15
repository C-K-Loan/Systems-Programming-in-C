#include "math.h"

#include <math.h>
#include <stdio.h>

void vec_print(vec_t *v) {
	printf("x: %f, y: %f, z: %f\n", v->x, v->y, v->z);
}

vec_t vec_add(vec_t *a, vec_t *b) {
	vec_t r;
	r.x = a->x + b->x;
	r.y = a->y + b->y;
	r.z = a->z + b->z;
	return r;
}

vec_t vec_sub(vec_t *a, vec_t *b) {
	vec_t r;
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	return r;
}

float vec_len(vec_t *v) {
	return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

float vec_dot(vec_t *a, vec_t *b) {
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

vec_t vec_cross(vec_t *a, vec_t *b) {
	vec_t r;
	r.x = a->y * b->z - a->z * b->y;
	r.y = a->z * b->x - a->x * b->z;
	r.z = a->x * b->y - a->y * b->x;

	return r;
}

vec_t vec_multiply(vec_t *v, float scale) {
	vec_t r;
	r.x = v->x * scale;
	r.y = v->y * scale;
	r.z = v->z * scale;

	return r;
}

vec_t vec_normalize(vec_t *v) {
	return vec_multiply(v, 1.0 / vec_len(v));
}

float vec_angle(vec_t *a, vec_t *b) {
	return vec_dot(a, b) / (vec_len(a) * vec_len(b));
}

vec_t vec_reflect(vec_t *v, vec_t *n) {
	float len2 = n->x * n->x + n->y * n->y + n->z * n->z;
	float u = -2.0 * (vec_dot(v, n) / len2);

	vec_t r;
	r.x = v->x + n->x * u;
	r.y = v->y + n->y * u;
	r.z = v->z + n->z * u;

	return vec_normalize(&r);
}

vec_t vec_rotate_x(vec_t *v, float angle) {
	float sin = sinf(deg2rad(angle));
	float cos = cosf(deg2rad(angle));

	vec_t r;
	r.x = v->x;
	r.y = v->y * cos - v->z * sin;
	r.z = v->y * sin + v->z * cos;
	return r;
}

vec_t vec_rotate_y(vec_t *v, float angle) {
	float sin = sinf(deg2rad(angle));
	float cos = cosf(deg2rad(angle));

	vec_t r;
	r.x = v->x * cos - v->z * sin;
	r.y = v->y;
	r.z = v->x * sin + v->z * cos;
	return r;
}

vec_t vec_rotate_z(vec_t *v, float angle) {
	float sin = sinf(deg2rad(angle));
	float cos = cosf(deg2rad(angle));

	vec_t r;
	r.x = v->x * cos - v->y * sin;
	r.y = v->x * sin + v->y * cos;
	r.z = v->z;
	return r;
}

int vec_refract(vec_t *v, vec_t *n, float ni, float no, vec_t *result) {
	vec_t norm = *n;
	if (vec_dot(v, n) > 0.0) {
		norm = vec_multiply(n, -1.0f);
	}
	float c1 = -vec_dot(&norm, v);

	float ns = ni / no;
	float c2 = 1 - ns * ns * (1 - c1 * c1);
	if (c2 < 0) {
		return 0;
	}
	c2 = sqrtf(c2);

	vec_t v1 = vec_multiply(v, ns);
	vec_t v2 = vec_multiply(&norm, ns * c1 - c2);

	*result = vec_add(&v1, &v2);
	return 1;
}

float deg2rad(float deg) {
	return deg * (PI / 180.0);
}

float rad2deg(float rad) {
	return rad * (180.0 / PI);
}
