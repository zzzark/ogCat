/*
@author: zrk
@date:   2020/11/6
*/
#pragma once
#include "mesh.h"
#include "material.h"
#include "texture2D.h"
#include "dynamic_mesh.h"
#include "glm/glm.hpp"

namespace cat
{
	class meshInstance final
	{
	protected:
		mesh* _ms = nullptr;
		static_mesh*  _sms = nullptr;
		dynamic_mesh* _dms = nullptr;
		texture2D _tex;

	protected:
		static const char* _s_header;
		static const char* _s_static;
		static const char* _s_dynamic;
		static const char* _s_model;
		static const char* _s_bones;
		static const char* _s_anims;
		static const char* _s_texture;
		static const char* _s_normal;
		static const char* _s_emm;
		static const char* _s_dif;
		static const char* _s_spe;
		static const char* _s_shi;
		static const char* _s_matrix;
		static const char* _s_bounding;

	public:
		material mtl;	// material
		glm::vec3 rec;	// recongnize
		glm::mat4 mdl;	// model view matrix

	public:
		bool loadStaticMesh(const char* filepath);
		bool loadDynamicMesh(const char* filepath, R3DBones& bones);
		bool loadTexture(const char* filepath);

		~meshInstance();

		void move(float dx, float dy, float dz);
		void moveTo(float x, float y, float z);
		void moveTo(const glm::vec3& pos);

		mesh* getMesh()     { return _ms; }
		static_mesh*  getStaticMesh()  { return _sms; }
		dynamic_mesh* getDynamicMesh() { return _dms; }
		texture2D& getTex() { return _tex; }

		const mesh* getMesh() const { return _ms; }
		const static_mesh* getStaticMesh() const { return _sms; }
		const dynamic_mesh* getDynamicMesh() const { return _dms; }
		const texture2D& getTex() const { return _tex; }
	};
}