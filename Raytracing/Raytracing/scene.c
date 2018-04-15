#include "scene.h"

#include <stdlib.h>

#include "geometry.h"

scene_t* create_scene() {
	scene_t *scene = (scene_t*) malloc(sizeof(scene_t));
	scene->geometry = list_create();

	// setup camera
	cam_t *cam = (cam_t*) malloc(sizeof(cam_t));
	cam->fov = 60.0;
	cam->pos.y = -5.0;
	cam->pos.z = -12.0;

	scene->cam = cam;

	// setup light
	light_t *light = (light_t*) malloc(sizeof(light_t));
	vec_t light_pos = { 0.0, 9.0, 0.0 };
	col_t light_col = { 1.0, 1.0, 1.0 };
	light->pos = light_pos;
	light->col = light_col;

	scene->light = light;

	// setup materials
	mat_t *mat_refl = create_material(0.0, 0.0, 0.0, 0.2, 0.5, 0.6, 32);
	mat_refl->refl = 0.7;
	mat_refl->shading = SMOOTH;

	mat_t *mat_refr = create_material(0.6, 0.8, 1.0, 0.2, 0.5, 0.6, 32);
	mat_refr->refr = 0.7;
	mat_refr->ior = 1.52;
	mat_refr->shading = SMOOTH;

	mat_t *mat_light = create_material(1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1);
	mat_t *ml[6];
	ml[0] = mat_light;
	ml[1] = mat_light;
	ml[2] = mat_light;
	ml[3] = mat_light;
	ml[4] = mat_light;
	ml[5] = mat_light;

	mat_t *red_wall = create_material(1.0, 0.0, 0.0, 0.1, 0.4, 0.0, 1);
	mat_t *green_wall = create_material(0.0, 1.0, 0.0, 0.1, 0.4, 0.0, 1);
	mat_t *white_wall = create_material(1.0, 1.0, 1.0, 0.1, 0.4, 0.0, 1);
	mat_t *wall[6];
	wall[0] = red_wall;
	wall[1] = green_wall;
	wall[2] = white_wall;
	wall[3] = white_wall;
	wall[4] = white_wall;
	wall[5] = white_wall;

	mat_t *mat_cat = create_material(1.0, 1.0, 1.0, 0.1, 0.4, 0.0, 1);
	mat_cat->shading = SMOOTH;

	// setup meshes
	mesh_t *mesh = create_cube((vec_t ) { -1.0, 9.9, -1.0 }, (vec_t ) { 1.0,
							9.95, 1.0 }, ml);
	list_add(scene->geometry, mesh, MESH);

	mesh = create_cube((vec_t ) { -8.0f, 0.0f, -6.0f }, (vec_t ) { 8.0f, 10.0f,
							6.0f }, wall);
	mesh_invert_normals(mesh);
	list_add(scene->geometry, mesh, MESH);

	mesh = create_icosasphere((vec_t ) { -3.0f, 2.01f, -1.0f }, 2.0, mat_refl,
			2);
	list_add(scene->geometry, mesh, MESH);

	mesh = create_icosasphere((vec_t ) { 3.0f, 2.01f, 1.5f }, 2.0, mat_refr, 2);
	list_add(scene->geometry, mesh, MESH);

	return scene;
}

void delete_scene(scene_t *scene) {
	if (scene == NULL) {
		return;
	}

	if (scene->cam != NULL) {
		free(scene->cam);
	}

	if (scene->light != NULL) {
		free(scene->light);
	}

	free(scene);
}

mat_t* create_material(float r, float g, float b, float amb, float diff,
		float spec, int shininess) {
	mat_t *mat = (mat_t*) malloc(sizeof(mat_t));
	mat->col = (col_t ) { r, g, b };
	mat->amb = amb;
	mat->diff = diff;
	mat->spec = spec;
	mat->shininess = shininess;

	return mat;
}
