/*
@author:	zrk
@date:		2020/7/3
*/
#pragma once
#include "mesh.h"
#include "R3DLoader.h"

namespace cat {

	class dynamic_mesh : protected R3DMesh, public mesh
	{
	protected:
		vao _secvao;
		vbo _secxyz;
		vbo _secstr;
		vbo _w;
		vbo _b;


		volatile float _fadeFactor = 0.0f;		// lastState ====> stop state
		glm::mat4 _lastMat;						// last transform matrix
		glm::mat4* _lastStateDelta = nullptr;	// last action
		glm::mat4* _currentState = nullptr;		// current action state

		unsigned int _state = 0;
		unsigned int _lastTime = 0; // last time
		unsigned int _accuTime = 0; // accumulated tiime 
		unsigned int _vtsCount = 0;	// for transformation feedback

		R3DBones* _bones = nullptr;
	public:
		friend class mesh_baker;

		void load(const char* binFile, R3DBones& bones);
		void update(R3DAnimation& ani, unsigned int tm, const glm::mat4& m);
		virtual ~dynamic_mesh();

		void loop();
		void pause();
		void stop(R3DAnimation& ani);
	};
	class mesh_baker
	{
		int _loc_bones = -1;
		fbShader _shd;
	public:
		void create();
		void bake(const vao& ao, unsigned int vtsCount, const vbo& vts, const vbo& nml, const glm::mat4* mat, unsigned int count);
		void bake(dynamic_mesh& ms, R3DBones& bones);
	};
};