#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct sphere_t
{
	uint	id;
	vec3	center;		// 2D position for translation
	float	radius=2.0f;		// radius
	float	rotation;
	float	position_x;
	float	position_y;
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update( float t);
};

inline std::vector<sphere_t> create_spheres(int num_spheres)
{
	std::vector<sphere_t> spheres;
	
	for (int k = 0; k < num_spheres; k++) {
		sphere_t s;
		vec3 center = vec3(0,0,0);
		float radius = (float)(1.0f - 0.08 * k);
		s = { uint(k),center, radius};
		
		spheres.emplace_back(s);
	}
	

	return spheres;
}

inline void sphere_t::update( float t)
{
	//radius = 1.0f;		// simple animation
	//theta	= t;
	//float rotation;
	float orbit_radius=4.0f;
	//float position_x;
	//float position_y;
	switch (id) {
	case 0:
		radius = 3.0f;
		rotation = 0.15f;
		orbit_radius = 0.003f;
		position_x = 0;
		position_y = 0;
		break;
	case 1:
		radius = 0.25f;
		rotation = 0.075f;
		orbit_radius = 4.00f;
		position_x = 4.5f;
		position_y = 3.0f;
		break;
	case 2:
		radius = 0.19f;
		rotation = -0.05f;
		orbit_radius = 1.33f;
		position_x = 5.5f;
		position_y = -2.0f;
		break;
	case 3:
		radius = 0.4f;
		rotation = 0.5f;
		orbit_radius = 1.0f;
		position_x = 7.0f;
		position_y = 9.0f;
		break;
	case 4:
		radius = 0.3f;
		rotation = .45f;
		orbit_radius = 0.75f;
		position_x = 8.0f;
		position_y = 4.0f;
		break;
	case 5:
		radius = 1.0f;
		rotation = 1.5f;
		orbit_radius = 0.3f;
		position_x = 10.0f;
		position_y = 5.0f;
		break;
	case 6:
		radius = 0.7f;
		rotation = 1.0f;
		orbit_radius = 0.2f;
		position_x = 12.0f;
		position_y = -4.0f;
		break;
	case 7:
		radius =0.5f;
		rotation = -0.7f;
		orbit_radius = 0.01f;
		position_x = 13.5f;
		position_y = 2.0f;
		break;
	case 8:
		radius = 0.6f;
		rotation = 0.7f;
		orbit_radius = 0.05f;
		position_x = 16.5f;
		position_y = -9.0f;
		break;
	}
	//float c	= cos(theta), s=sin(theta);
	model_matrix = mat4::rotate(vec3(0, 0, 1), t*rotation)* mat4::translate(vec3(position_x, position_y, 0))*mat4::scale(vec3(radius,radius,radius));
	// these transformations will be explained in later transformation lecture
	/*mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		c,-s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;*/
}

#endif
