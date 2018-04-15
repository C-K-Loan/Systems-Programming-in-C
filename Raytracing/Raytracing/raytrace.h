#ifndef RAYTRACE_H_
#define RAYTRACE_H_

#include "color.h"
#include "geometry.h"
#include "math.h"
#include "scene.h"

static const int WIDTH = 800;
static const int HEIGHT = 600;
static const int RECURSION_LIMIT = 5;
static const int SHADOWS = 1;

/**
  * Renders a scene of two speheres in a room with one light source and saves
  * the resulting image as Bitmap in the passed buffer. Each pixel is represented
  * as an Element of type pix_t.
  * With the parameters offset_x, offset_y, frame_width and frame_height it's
  * possible to just calculate a smaller part of the whole scene.
  *
  * img:          Pointer to a frame_width * frame_height large memory area.
  *               The data is written row-wise, all pixel of the first row [x - (x + frame_width), y], followed
  *               by pixel of the second row [x - (x + frame_width), y + 1], and so on
  *               till [x - (x + frame_width), y + frame_height].
  * offset_x:     x offset of the area to calculate
  * offset_y:     y offset of the area to calculate
  * frame_width:  Width of the area to calculate
  * frame_height: Height of the area to calculate
  */
void raytrace(pix_t *img, vec_t *bounds, scene_t *scene, int offset_x, int offset_y, int frame_width, int frame_height);

void calculate_casting_bounds(cam_t *cam, vec_t *bounds);

ray_t create_primary_ray(vec_t *bounds, int x, int y);

col_t trace_ray(ray_t *ray, scene_t *scene);

int intersect_scene(ray_t *ray, scene_t *scene, intersection_t *result);

int intersect_sphere(ray_t *ray, sphere_t *sphere, intersection_t *result);

int intersect_mesh(ray_t *ray, mesh_t *mesh, intersection_t *result);

int intersect_triangle(ray_t *ray, tri_t *tri, intersection_t *result);

intersection_t* get_closest(intersection_t *a, intersection_t *b);

col_t calculate_color(intersection_t *intersection, scene_t *scene);

int calculate_shadow(intersection_t *intersection, scene_t *scene);

int calculate_refraction(intersection_t *i, float ior_old, float ior_new,
		scene_t *scene, intersection_t *result);

#endif
