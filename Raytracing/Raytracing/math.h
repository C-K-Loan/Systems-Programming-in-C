#ifndef MATH_H_
#define MATH_H_

#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))

static const float PI = 3.1415927;

/* vector data structure */
typedef struct {
	float x;
	float y;
	float z;
} vec_t;

void vec_print(vec_t *v);

vec_t vec_add(vec_t *a, vec_t *b);

vec_t vec_sub(vec_t *a, vec_t *b);

float vec_len(vec_t *v);

float vec_dot(vec_t *a, vec_t *b);

vec_t vec_cross(vec_t *a, vec_t *b);

vec_t vec_multiply(vec_t *v, float scale);

vec_t vec_normalize(vec_t *v);

float vec_angle(vec_t *a, vec_t *b);

vec_t vec_reflect(vec_t *v, vec_t *n);

vec_t vec_rotate_x(vec_t *v, float angle);

vec_t vec_rotate_y(vec_t *v, float angle);

vec_t vec_rotate_z(vec_t *v, float angle);

int vec_refract(vec_t *v, vec_t *n, float ni, float no, vec_t *result);

float deg2rad(float deg);

float rad2deg(float rad);

#endif
