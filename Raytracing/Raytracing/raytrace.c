#include "raytrace.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "list.h"
#include <string.h>

void raytrace(pix_t *img, vec_t *bounds, scene_t *scene, int offset_x, int offset_y, int frame_width, int frame_height){

	int x, y;
	for (x=offset_x; x<offset_x+frame_width; x++) {
        	for (y=offset_y; y<offset_y+frame_height; y++) {

			// cast a ray from the camera into the scene
			// at coordinates (x, y)
			ray_t ray = create_primary_ray(bounds, x, y);

			// do raytracing for the pixel (x, y)
			// -> calculate pixel color
			col_t col = trace_ray(&ray, scene);

			// convert color to pixel information
			pix_t pix = col2pix(&col);
			
			// write pixel to buffer
			img[(y-offset_y)*frame_width + (x-offset_x)] = pix;
		}
	}
}

void calculate_casting_bounds(cam_t *cam, vec_t *bounds) {

	float aspectRatio = (float) WIDTH / (float) HEIGHT;
	float x = tanf(deg2rad(cam->fov / 2.0));

	// create the 4 bounds
	vec_t topRight = { x * aspectRatio, x, -1.0 };
	vec_t topLeft = { -x * aspectRatio, x, -1.0 };
	vec_t bottomLeft = { -x * aspectRatio, -x, -1.0 };
	vec_t origin = vec_multiply(&(cam->pos), -1.0f);

	topRight = vec_add(&topRight, &origin);
	topLeft = vec_add(&topLeft, &origin);
	bottomLeft = vec_add(&bottomLeft, &origin);
	topRight = vec_rotate_x(&topRight, -cam->rot.x);
	topRight = vec_rotate_y(&topRight, cam->rot.y);
	topLeft = vec_rotate_x(&topLeft, -cam->rot.x);
	topLeft = vec_rotate_y(&topLeft, cam->rot.y);
	bottomLeft = vec_rotate_x(&bottomLeft, -cam->rot.x);
	bottomLeft = vec_rotate_y(&bottomLeft, cam->rot.y);
	origin = vec_rotate_x(&origin, -cam->rot.x);
	origin = vec_rotate_y(&origin, cam->rot.y);
	vec_t absX = vec_sub(&topRight, &topLeft);
	vec_t absY = vec_sub(&bottomLeft, &topLeft);
	vec_t xStep = vec_multiply(&absX, 1.0f / (float) (WIDTH - 1));
	vec_t yStep = vec_multiply(&absY, 1.0f / (float) (HEIGHT - 1));

	bounds[0] = origin;
	bounds[1] = topLeft;
	bounds[2] = xStep;
	bounds[3] = yStep;
}

ray_t create_primary_ray(vec_t *bounds, int x, int y) {
	vec_t *origin = bounds;

	vec_t a = vec_multiply(bounds + 2, x);
	a = vec_add(bounds + 1, &a);
	vec_t b = vec_multiply(bounds + 3, HEIGHT - y);

	vec_t direction = vec_add(&a, &b);
	direction = vec_sub(&direction, origin);

	ray_t ray;
	ray.origin = *origin;
	ray.direction = direction;
	ray.recursion = 0;

	return ray;
}

col_t trace_ray(ray_t *ray, scene_t *scene) {
	col_t col = { 0.0, 0.0, 0.0 };
	intersection_t i;
	memset(&i,0,sizeof(intersection_t));

	int test = intersect_scene(ray, scene, &i);
	if (test) {
		col = calculate_color(&i, scene);
	}
	return col;
}

int intersect_scene(ray_t *ray, scene_t *scene, intersection_t *result) {
	intersection_t in;
	intersection_t curr;

	int init = 0;

	node_t *node = scene->geometry->root;
	while (node) {

		int ret = 0;
		if (node->type == MESH) {
			ret = intersect_mesh(ray, (mesh_t*) node->data, &curr);
		} else if (node->type == SPHERE) {
			ret = intersect_sphere(ray, (sphere_t*) node->data, &curr);
		}

		if (!ret) {
			node = node->next;
			continue;
		} else {
			if (!init) {
				in = curr;
				init = 1;
			}

			intersection_t *closest = get_closest(&in, &curr);

			in = *closest;
		}
		node = node->next;
	}

	*result = in;

	return init;
}

/* code from http://wiki.cgsociety.org/index.php/Ray_Sphere_Intersection */
int intersect_sphere(ray_t *ray, sphere_t *sphere, intersection_t *result) {
	// Compute A, B and C coefficients
	vec_t o = vec_sub(&(ray->origin), &(sphere->pos));
	vec_t d = ray->direction;
	float a = vec_dot(&d, &d);
	float b = 2 * vec_dot(&d, &o);
	float c = vec_dot(&o, &o) - (sphere->radius * sphere->radius);

	// Find discriminant
	float disc = b * b - 4 * a * c;

	// if discriminant is negative there are no real roots, so return
	// false as ray misses sphere
	if (disc < 0)
		return 0;

	// compute q as described above
	float distSqrt = sqrtf(disc);
	float q;
	if (b < 0)
		q = (-b - distSqrt) / 2.0;
	else
		q = (-b + distSqrt) / 2.0;

	// compute t0 and t1
	float t0 = q / a;
	float t1 = c / q;

	// make sure t0 is smaller than t1
	if (t0 > t1) {
		// if t0 is bigger than t1 swap them around
		float temp = t0;
		t0 = t1;
		t1 = temp;
	}

	// if t1 is less than zero, the object is in the ray's negative
	// direction
	// and consequently the ray misses the sphere
	if (t1 < 0)
		return 0;
	float t;

	// if t0 is less than zero, the intersection point is at t1
	if (t0 < 0) {
		t = t1;
	}

	// else the intersection point is at t0
	else {
		t = t0;
	}

	vec_t isv = vec_multiply(&(ray->direction), t);
	isv = vec_add(&isv, &(ray->origin));

	vec_t normal = vec_sub(&isv, &(sphere->pos));
	normal = vec_normalize(&normal);

	result->pos = isv;
	result->ray = ray;
	result->mat = sphere->mat;
	result->n = normal;
	result->sphere = sphere;
	result->type = SPHERE;

	return 1;
}

int intersect_mesh(ray_t *ray, mesh_t *mesh, intersection_t *result) {
	intersection_t in;
	intersection_t curr;

	int init = 0;

	int i;
	for (i = 0; i < mesh->num_tris; i++) {
		vec_t n = mesh->tris[i].n;
		if (vec_dot(&(ray->direction), &n) > 0) {
			continue;
		}

		int ret = intersect_triangle(ray, mesh->tris + i, &curr);

		if (!ret) {
			continue;
		} else {
			if (!init) {
				in = curr;
				init = 1;
			}

			intersection_t *closest = get_closest(&in, &curr);

			in = *closest;
		}
	}

	*result = in;

	return init;
}

int intersect_triangle(ray_t *ray, tri_t *tri, intersection_t *result) {
	vec_t t_vec, u_vec, q_vec;

	vert_t *verts = tri->mesh->verts;
	vec_t *a = &(verts[tri->a].p);
	vec_t *b = &(verts[tri->b].p);
	vec_t *c = &(verts[tri->c].p);

	vec_t *d = &(ray->direction);
	vec_t *o = &(ray->origin);

	float det, invDet;

	vec_t edge1 = vec_sub(b, a);
	vec_t edge2 = vec_sub(c, a);

	u_vec = vec_cross(d, &edge2);
	det = vec_dot(&edge1, &u_vec);

	if (det > -0.00001f && det < 0.00001f) {
		return 0;
	}

	invDet = 1.0f / det;

	t_vec = vec_sub(o, a);

	float u = vec_dot(&t_vec, &u_vec) * invDet;
	if (u < 0.0 || u > 1.0) {
		return 0;
	}

	q_vec = vec_cross(&t_vec, &edge1);

	float v = vec_dot(d, &q_vec) * invDet;
	if (v < 0.0 || u + v > 1.0) {
		return 0;
	}

	float t = vec_dot(&edge2, &q_vec) * invDet;

	if (t < 0.0) {
		return 0;
	}

	vec_t ip = vec_multiply(d, t);
	ip = vec_add(o, &ip);

	result->pos = ip;
	result->ray = ray;
	result->mat = tri->mat;

	// if flat shaded use face normal
	result->n = tri->n;

	// if smooth shaded, calculate normal by vertex normals
	if (tri->mat->shading == SMOOTH) {
		vec_t *a = &(verts[tri->a].n);
		vec_t *b = &(verts[tri->b].n);
		vec_t *c = &(verts[tri->c].n);

		vec_t t1 = vec_multiply(a, 1 - u - v);
		vec_t t2 = vec_multiply(b, u);

		t1 = vec_add(&t1, &t2);
		t2 = vec_multiply(c, v);

		result->n = vec_add(&t1, &t2);
	}

	result->mesh = tri->mesh;
	result->type = MESH;
	result->u = u;
	result->v = v;

	return 1;
}

intersection_t* get_closest(intersection_t *a, intersection_t *b) {
	if (a == NULL) {
		return b;
	}

	if (b == NULL) {
		return a;
	}

	vec_t tmp = vec_sub(&(a->pos), &(a->ray->origin));
	float distA = vec_len(&tmp);

	tmp = vec_sub(&(b->pos), &(b->ray->origin));
	float distB = vec_len(&tmp);

	if (distA < 0.01) {
		return b;
	}
	if (distB < 0.01) {
		return a;
	}

	return distA < distB ? a : b;
}

col_t calculate_color(intersection_t *intersection, scene_t *scene) {

	int cast_shadow;

	if (SHADOWS) {
		cast_shadow = calculate_shadow(intersection, scene);
	}

	mat_t *mat = intersection->mat;
	col_t col = mat->col;

	vec_t *pos = &(intersection->pos);
	vec_t *light_pos = &(scene->light->pos);

	vec_t to_eye = vec_sub(&(intersection->ray->origin), pos);
	to_eye = vec_normalize(&to_eye);

	vec_t *n = &(intersection->n);

	float specular = 0.0;
	float diffuse = 0.0;

	// calculate diffuse and specular only when not shadowed
	if (vec_dot(&to_eye, n) > 0) {
		// calculate diffuse light
		vec_t to_light = vec_sub(light_pos, pos);
		to_light = vec_normalize(&(to_light));

		diffuse = max(0.0, vec_angle(n, &to_light));
		diffuse *= mat->diff;

		// calculate specular highlights
		vec_t from_light = vec_sub(pos, light_pos);
		from_light = vec_normalize(&from_light);

		vec_t reflected_light = vec_reflect(&from_light, n);
		reflected_light = vec_normalize(&reflected_light);

		float specular_dot = vec_dot(&to_eye, &reflected_light);

		if (specular_dot > 0) {
			specular = pow(specular_dot, mat->shininess);
		}

		specular *= mat->spec;
	}

	col_t rgb;
	rgb.r = col.r * diffuse + scene->light->col.r * specular;
	rgb.g = col.g * diffuse + scene->light->col.g * specular;
	rgb.b = col.b * diffuse + scene->light->col.b * specular;

	rgb.r = col.r * mat->amb + rgb.r * cast_shadow;
	rgb.g = col.g * mat->amb + rgb.g * cast_shadow;
	rgb.b = col.b * mat->amb + rgb.b * cast_shadow;

	rgb.r = min(1.0, rgb.r);
	rgb.g = min(1.0, rgb.g);
	rgb.b = min(1.0, rgb.b);

	col_t refl_col;
	refl_col.r = 0.0;
	refl_col.g = 0.0;
	refl_col.b = 0.0;

	float refl = mat->refl;

	// calculate reflection
	if (refl > 0 && intersection->ray->recursion <= RECURSION_LIMIT) {
		

		vec_t refl_vec = vec_reflect(&(intersection->ray->direction), n);
		refl_vec = vec_normalize(&refl_vec);

		ray_t refl_ray;
		refl_ray.origin = *pos;
		refl_ray.direction = refl_vec;
		refl_ray.recursion = intersection->ray->recursion + 1;

		intersection_t refl_intersec;
		int hit = intersect_scene(&refl_ray, scene, &refl_intersec);
		if (hit) {
			refl_col = calculate_color(&refl_intersec, scene);
		}
	}

	// merge color with reflected color
	rgb.r = rgb.r * (1 - refl) + refl_col.r * refl;
	rgb.g = rgb.g * (1 - refl) + refl_col.g * refl;
	rgb.b = rgb.b * (1 - refl) + refl_col.b * refl;

	col_t refr_col;
	refr_col.r = 0.0;
	refr_col.g = 0.0;
	refr_col.b = 0.0;

	float refr = mat->refr;

	// calculate refraction
	if (refr > 0 && intersection->ray->recursion <= RECURSION_LIMIT) {

		intersection_t refr_intersec;
		int hit = calculate_refraction(intersection, 1.0, mat->ior, scene,
				&refr_intersec);

		if (hit) {
			refr_col = calculate_color(&refr_intersec, scene);
		}
	}

	// merge color with refracted color
	rgb.r = rgb.r * (1 - refr) + refr_col.r * refr;
	rgb.g = rgb.g * (1 - refr) + refr_col.g * refr;
	rgb.b = rgb.b * (1 - refr) + refr_col.b * refr;

	return rgb;
}

int calculate_shadow(intersection_t *intersection, scene_t *scene) {
	vec_t pos = intersection->pos;
	vec_t light_pos = scene->light->pos;
	vec_t to_light = vec_sub(&light_pos, &pos);
	to_light = vec_normalize(&to_light);

	ray_t secondaryRay;
	secondaryRay.origin = pos;
	secondaryRay.direction = to_light;

	intersection_t intersect;
	int hit = intersect_scene(&secondaryRay, scene, &intersect);
	if (hit) {
		vec_t a = vec_sub(&light_pos, &pos);
		vec_t b = vec_sub(&(intersect.pos), &pos);
		if (vec_len(&b) < vec_len(&a)) {
			return 0;
		}
	}

	return 1;
}

int calculate_refraction(intersection_t *i, float ior_old, float ior_new,
		scene_t *scene, intersection_t *result) {

	// entering
	vec_t v;
	vec_refract(&(i->ray->direction), &(i->n), ior_old, ior_new, &v);
	v = vec_normalize(&v);

	ray_t ray;
	vec_t tmp;
	if (i->type == MESH) {
		tmp = vec_multiply(&v, i->mesh->size * 2.0);
	} else if (i->type == SPHERE) {
		tmp = vec_multiply(&v, i->sphere->size * 2.0);
	}

	ray.origin = vec_add(&(i->pos), &tmp);
	ray.direction = vec_multiply(&v, -1.0);
	ray.recursion = i->ray->recursion + 1;

	if (i->type == MESH) {
		intersect_mesh(&ray, i->mesh, i);
	} else if (i->type == SPHERE) {
		intersect_sphere(&ray, i->sphere, i);
	}

	// exiting
	vec_t v2, v3;
	int ret = vec_refract(&v, &(i->n), ior_new, ior_old, &v2);

	while (!ret) {
		v3 = vec_multiply(&(i->n), -1.0f);
		v2 = vec_reflect(&v, &v3);
		ray_t ray;
		vec_t tmp;
		if (i->type == MESH) {
			tmp = vec_multiply(&v2, i->mesh->size * 2.0);
		} else if (i->type == SPHERE) {
			tmp = vec_multiply(&v2, i->sphere->size * 2.0);
		}

		ray.origin = vec_add(&(i->pos), &tmp);
		ray.direction = vec_multiply(&v2, -1.0);
		ray.recursion = i->ray->recursion + 1;

		if (i->type == MESH) {
			intersect_mesh(&ray, i->mesh, i);
		} else if (i->type == SPHERE) {
			intersect_sphere(&ray, i->sphere, i);
		}

		ret = vec_refract(&v2, &(i->n), ior_new, ior_old, &v2);
	}
	v = vec_normalize(&v2);

	ray.origin = i->pos;
	ray.direction = v2;
	ray.recursion = i->ray->recursion + 1;

	return intersect_scene(&ray, scene, result);
}
