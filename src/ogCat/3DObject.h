#pragma once
#include "material.h"
#include "texture2D.h"
#include "mesh.h"
#include "dynamic_mesh.h"
#include "renderer.h"
#include "FXAA.h"
#include "skybox.h"
#include "terrain.h"
#include "shadowMap2D.h"
#include "shadowMapCUBE.h"
#include "shadowBuffer.h"
#include "glm/glm.hpp"
#include "TwoPassBlur.h"

namespace cat
{

	/*
	R3DINFO DYNAMIC
	model   res/textures/hell.bin
	bones   res/textures/bn.bon
	anims   res/textures/an.ani
	texture res/textures/particle.jpg
	normal  0
	emm     0.0 0.0 0.0
	dif     0.5 0.5 0.5
	spe     0.1 0.1 0.1
	shi     5.0
	matrix
	1 0 0 0
	0 1 0 0
	0 0 1 0
	0 0 0 1
	bounding 2
	0 0 0    // base
	1        // x
	1	     // y
	1    	 // z
	0 0 1    // base
	1        // x
	1	     // y
	1    	 // z

	*/
	class tdobj
	{
	protected:
		class bounding {
		public:
			struct box {
				glm::vec3 _base;
				float _xdir;
				float _ydir;
				float _zdir;
				glm::vec3 _p[8];
			};
			glm::mat4 _mat;
			unsigned int _count = 0;
			box* _boxes = nullptr;
			bool collide(const bounding& static_obj) const;
			~bounding();
		};
		material  _mtl;
		texture2D _tex;
		static_mesh*  _sms = nullptr;
		dynamic_mesh* _dms = nullptr;

	public:
		bounding _bnd;
		friend class world;

		void load(const char* filepath);
		void draw();
		void free();
		virtual ~tdobj();
		void move(float dx, float dy, float dz);
		void moveAbs(float x, float y, float z);
		void setMat(const glm::mat4& m);
		inline bool collide(tdobj* obj) const { return _bnd.collide(obj->_bnd); }
		inline const glm::mat4& getMat() const { return _bnd._mat; }
		inline const material& getMtl()  const { return _mtl; }
		inline const texture2D& getTex() const { return _tex; }
		inline mesh* getMesh()                { if (_sms == nullptr) return _dms; else  return _sms; }
		inline static_mesh*  getStaticMesh()  { return _sms; }
		inline dynamic_mesh* getDynamicMesh() { return _dms; }
	};
}