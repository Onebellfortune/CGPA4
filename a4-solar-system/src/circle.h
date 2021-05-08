#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	uint	id;
	vec3	center;		// 2D position for translation
	float	radius = 1.0f;		// radius
	float	rotation;
	float	position_x;
	float	position_y;
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float t);
};

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

inline std::vector<circle_t> create_circles(int N)
{
	std::vector<circle_t> circles;
	
	for (int i = 0; i < N; i++) {
		circle_t c;
		vec3 center = vec3(0, 0, 0);
		float radius = 1.0f;
		c = { uint(i),center,radius};
		circles.emplace_back(c);
	}
	
	return circles;
}

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
inline float radx(float t, float r, float d) {
	return r * cos(t+d) - r * sin(t+d);
}
inline float rady(float t, float r, float d) {
	return r * cos(t+d) + r * sin(t+d);
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
	case 0: // 태양
		radius = 3.0f;
		rotation = 0.5f;
		orbit_radius = 0.003f;
		position_x = 0;
		position_y = 0;
		break;
	case 1: // 수성
		radius = 0.25f;
		rotation = 0.075f;
		orbit_radius = 4.00f;
		position_x = 4.5f;
		position_y = 3.0f;
		break;
	case 2: // 금성
		radius = 0.19f;
		rotation = -0.05f;
		orbit_radius = 1.33f;
		position_x = 5.5f;
		position_y = -2.0f;
		break;
	case 3:	//지구에 한개
		radius = 0.4f;
		rotation = 0.5f;
		orbit_radius = 1.0f;
		position_x = 7.0f;
		position_y = 9.0f;
		break;
	case 9: // 달
		radius = 0.3f;
		rotation = 0.5f;
		position_x = 7.0f;
		position_y = 9.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(cos(2.5f*t-1.0f)-sin(2.5f * t - 1.0f), cos(2.5f * t - 1.0f)+sin(2.5f * t - 1.0f),0)) * model_matrix;
		break;
	case 4: //화성,
		radius = 0.3f;
		rotation = .45f;
		orbit_radius = 0.75f;
		position_x = 8.0f;
		position_y = 4.0f;
		break;
	case 5: //목성 4개
		radius = 1.0f;
		rotation = 0.75f;
		orbit_radius = 0.3f;
		position_x = 14.0f;
		position_y = 5.0f;
		break;
	case 10:
		radius = 0.2f;
		rotation = 0.75f;
		position_x = 14.0f;
		position_y = 5.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(cos(2.5f * t - 1.0f) - sin(2.5f * t - 1.0f), cos(2.5f * t - 1.0f) + sin(2.5f * t - 1.0f), 0)) * model_matrix;

		break;
	case 11:
		radius = 0.25f;
		rotation = 0.75f;
		position_x = 14.0f;
		position_y = 5.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(cos(2.5f * t - 2.0f) - sin(2.5f * t - 2.0f), cos(2.5f * t - 2.0f) + sin(2.5f * t - 2.0f), 0)) * model_matrix;

		break;
	case 12:
		radius = 0.2f;
		rotation = 0.75f;
		position_x = 14.0f;
		position_y = 5.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(radx(t*2.5f,1.2f,-2.5f), rady(t * 2.5f, 1.2f, -2.5f), 0)) * model_matrix;

		break;
	case 13:
		radius = 0.3f;
		rotation = 0.75f;
		position_x = 14.0f;
		position_y = 5.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(radx(t * 2.5f, 1.0f,0), rady(t * 2.5f, 1.0f,0), 0)) * model_matrix;
		break;
	case 6: // 토성
		radius = 0.7f;
		rotation = 0.4f;
		orbit_radius = 0.2f;
		position_x = 17.0f;
		position_y = -4.0f;
		break;
	case 7: //천왕성 5개
		radius =0.5f;
		rotation = -0.35f;
		orbit_radius = 0.01f;
		position_x = 19.5f;
		position_y = 2.0f;
		break;
	case 14:
		radius = 0.2f;
		rotation = -0.35f;
		orbit_radius = 0.01f;
		position_x = 19.5f;
		position_y = 2.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(radx(t * 2.5f, 1.0f, 0), rady(t * 2.5f, 1.0f, 0), 0)) * model_matrix;
		break;
	case 15:
		radius = 0.1f;
		rotation = -0.35f;
		orbit_radius = 0.01f;
		position_x = 19.5f;
		position_y = 2.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(radx(t * 1.5f, 1.5f, 1.0f), rady(t * 1.5f, 1.5f, 1.0f), 0)) * model_matrix;
		break;
	case 16:
		radius = 0.1f;
		rotation = -0.35f;
		orbit_radius = 0.01f;
		position_x = 19.5f;
		position_y = 2.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(radx(t * 2.5f, 1.0f, -1.0f), rady(t * 2.5f, 1.0f, -1.0f), 0)) * model_matrix;
		break;
	case 17: 
		radius = 0.2f;
		rotation = -0.35f;
		orbit_radius = 0.01f;
		position_x = 19.5f;
		position_y = 2.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(radx(-t * 2.5f, 1.8f, 0), rady(-t * 2.5f, 1.8f, 0), 0)) * model_matrix;
		break;
	case 18:
		radius = 0.2f;
		rotation = -0.35f;
		orbit_radius = 0.01f;
		position_x = 19.5f;
		position_y = 2.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(radx(t * 2.5f, 0.6f, 2.0f), rady(t * 2.5f, 0.6f, 2.0f), 0)) * model_matrix;
		break;

	case 8: //해왕성 2개
		radius = 0.6f;
		rotation = 0.35f;
		orbit_radius = 0.05f;
		position_x = 22.5f;
		position_y = -9.0f;
		break;
	case 19:
		radius = 0.15f;
		rotation = 0.35f;
		orbit_radius = 0.05f;
		position_x = 22.5f;
		position_y = -9.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(radx(-t * 2.5f, 0.6f, 2.0f), rady(-t * 2.5f, 0.6f, 2.0f), 0)) * model_matrix;
		break;
	case 20:
		radius = 0.25f;
		rotation = 0.35f;
		orbit_radius = 0.05f;
		position_x = 22.5f;
		position_y = -9.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0));
		model_matrix = model_matrix * mat4::scale(vec3(radius, radius, radius));
		model_matrix = mat4::translate(vec3(radx(t * 2.5f, 1.6f, 0.0f), rady(t * 2.5f, 1.6f, 0.0f), 0)) * model_matrix;
		break;
	case 21:
		radius = 1.7f;
		rotation = 0.4f;
		position_x = 17.0f;
		position_y = -4.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0)) * mat4::scale(vec3(radius, radius, 0.01f));
		break;
	case 22:
		radius = 1.6f;
		rotation = 0.35f;
		position_x = 22.5f;
		position_y = -9.0f;
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0)) * mat4::scale(vec3(radius, radius, 0.01f));
		break;
	}
	//float c	= cos(theta), s=sin(theta);
	if (id < 9) {
		model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0)) * mat4::scale(vec3(radius, radius, radius));
	}
	
	
}



inline void circle_t::update(float t)
{
	// these transformations will be explained in later transformation lecture
	if (id == 0) { //토성고리
		radius = 1.7f;
		rotation = 0.4f;
		position_x = 17.0f;
		position_y = -4.0f;
	}
	else if (id == 1) { // 천왕성고리
		radius = 1.6f;
		rotation = 0.35f;
		position_x = 22.5f;
		position_y = -9.0f;
	}
	model_matrix = mat4::rotate(vec3(0, 0, 1), t * rotation) * mat4::translate(vec3(position_x, position_y, 0)) * mat4::scale(vec3(radius, radius, radius));

	
}

#endif
