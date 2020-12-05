#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "camera.h"
namespace glm
{
	static mat4 perspective(float nearZ, float farZ, float fov, float aspect)
	{
		float top = nearZ * sin(fov / 2) / cos(fov / 2);
		float right = top * aspect;
		return transpose(mat4(
			vec4(nearZ / right, 0,            0,                                0                                   ),
			vec4(0,             nearZ / top,  0,                                0                                   ),
			vec4(0,             0,           -(farZ + nearZ) / (farZ - nearZ), -2.0f * farZ * nearZ / (farZ - nearZ)),
			vec4(0,             0,           -1.0f,                             0                                   )
		));
	}
	static mat4 camera(const vec3& eye, const vec3& at, const vec3& up)
	{
		vec3 n = eye - at;		// z
		vec3 v = cross(up, n);	// x
		vec3 u = cross(n, v);	// y
		n = normalize(n);
		v = normalize(v);
		u = normalize(u);
		return transpose(mat4(
			vec4(v.x, v.y, v.z, -(eye.x * v.x + eye.y * v.y + eye.z * v.z)),
			vec4(u.x, u.y, u.z, -(eye.x * u.x + eye.y * u.y + eye.z * u.z)),
			vec4(n.x, n.y, n.z, -(eye.x * n.x + eye.y * n.y + eye.z * n.z)),
			vec4(0.0f, 0.0f, 0.0f, 1.0f)
		));
	}
	static mat4 camera(const vec3& eye, const vec3& at) {
		return camera(eye, at, vec3(0, 1, 0));
	}
}

const glm::mat4& cat::camera::ortho(float nx, float fx, float ny, float fy, float nz, float fz)
{
	_proj = glm::ortho(nx, fx, ny, fy, nz, fz);
	_combined = _proj * _view;
	return _proj;
}

const glm::mat4& cat::camera::perspective(float nearZ, float farZ, float fov, float aspect)
{
	_proj = glm::perspective(nearZ, farZ, fov, aspect);
	_combined = _proj * _view;
	return _proj;
}

const glm::mat4& cat::camera::look(const glm::vec3& at)
{
	_at = at;
	_view = glm::camera(_eye, _at);
	_combined = _proj * _view;
	return _view;
}

const glm::mat4& cat::camera::walk(const glm::vec3& pos)
{
	_eye = pos;
	_view = glm::camera(_eye, _at);
	_combined = _proj * _view;
	return _view;
}