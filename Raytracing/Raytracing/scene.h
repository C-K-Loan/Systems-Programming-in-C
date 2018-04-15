#ifndef SCENE_H_
#define SCENE_H_

#include "color.h"
#include "list.h"
#include "math.h"

static const int FLAT = 0;
static const int SMOOTH = 1;

typedef struct {
	vec_t pos;
	col_t col;
} light_t;

typedef struct {
	vec_t pos;
	vec_t rot;
	float fov;
} cam_t;

typedef struct {
	col_t col;
	float amb;
	float diff;

	float spec;
	int shininess;

	float refl;
	float refr;
	float ior;

	int shading;
} mat_t;

typedef struct {
	light_t *light;
	cam_t *cam;

	list_t *geometry;
} scene_t;

scene_t* create_scene();

void delete_scene(scene_t *scene);

mat_t* create_material(float r, float g, float b, float amb, float diff,
		float spec, int shininess);

#endif
