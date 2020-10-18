/*
@author:	zrk
@date:		2020/6/3
*/

#pragma once
#include "glm/glm.hpp"

namespace cat
{
	class camera
	{
		glm::vec3 _eye;
		glm::vec3 _at;
		glm::mat4 _proj;
		glm::mat4 _view;
		glm::mat4 _combined;
	public:
		camera() : _proj(1.0f), _view(1.0f), _combined(1.0f), _eye(0, 0, 1), _at(0, 0, 0) {}
		const glm::mat4& proj() const { return _proj; }
		const glm::mat4& view() const { return _view; }
		const glm::mat4& comb() const { return _combined; }
		glm::mat4 mvp(const glm::mat4& world) const { return _combined * world; }

		const glm::mat4& ortho(float nx, float fx, float ny, float fy, float nz, float fz);
		const glm::mat4& perspective(float nearZ, float farZ, float fov, float aspect);
		const glm::mat4& look(const glm::vec3& at);
		const glm::mat4& walk(const glm::vec3& pos);
		inline const glm::vec3& getEye() const { return _eye; }
		inline const glm::vec3& getAt() const { return _at; }
		const glm::mat4& delta(float dx, float dy, float dz, float dphi, float dtheta);
	};
}