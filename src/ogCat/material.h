/*
@author:	zrk
@date:		2020/6/5
*/
#pragma once
#include "glm/glm.hpp"
namespace cat
{
	struct light
	{
		glm::vec3 ambient;
		glm::vec3 color;
	};
	struct directionalLight : public light
	{
		glm::vec3 direction;
		float intensity;
	};
	struct pointLight : public light
	{
		glm::vec3 position;
		float intensity;
		inline float maxRange(float lowV = 0.99f) { return sqrt(intensity / lowV); }
	};
	struct material
	{
		// c = e + d * kd + s * pow(ks, sh)
		glm::vec3 emmisive;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};
}