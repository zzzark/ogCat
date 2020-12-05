/*
//@author:	zark
//@time:	2020 / 6 / 1
//note:
// R3DLoader can both load 'static' mesh and 'dynamic' mesh('dynamic' mesh is a 'static' mesh with animation)
*/
#pragma once
#include <string>
#include "glm/glm.hpp"
namespace cat
{
	class R3DLoader
	{
		float*			_xyz;	// x y z
		float*			_uv;	// u v
		float*			_str;	// s t r
		unsigned int*	_b;		// boneID * 4
		float*			_w;		// weight * 4

		unsigned int* _ids;

		__int32 _vtsCnt;
		__int32 _idsCnt;
	public:
		R3DLoader();
		virtual ~R3DLoader();

		void load(const char* filepath);
		void load(const char* ptr, size_t size);

		void free();

		inline const float* getXYZ() const { return _xyz; }
		inline const float* getUV()  const { return _uv; }
		inline const float* getSTR() const { return _str; }
		inline const float* getW()   const { return _w; }
		inline const unsigned int* getB() { return _b; }
		inline const unsigned int* getIds() { return _ids; }

		inline const __int32 getVtsCnt() { return _vtsCnt; }
		inline const __int32 getIdsCnt() { return _idsCnt; }

		static const unsigned int MAX_BONE_COUNT = 4;

	};
	class R3DBones
	{
	public:
		class bone;
	protected:
		bone* _root = nullptr;
		glm::mat4* _ori = nullptr;
		glm::mat4* _trs = nullptr;
		glm::mat4* _off = nullptr;
		glm::mat4* _cmb = nullptr;
		unsigned int _count = 0;
	public:
		virtual ~R3DBones() {
			free();
		}
		void free();
		void load(const char* filepath);
		void load(const char* ptr, size_t size);
		void update(const glm::mat4& m);
		void reset();
		int getIndex(const char* name) const;
		glm::mat4* getTransformationMatrix() const { return _trs; }
		glm::mat4* getCombMatrix()           const { return _cmb; }
		unsigned int getCount()              const { return _count; }
	};
	class R3DAnimation
	{
	protected:
		class animation {
		public:
			unsigned int _index = 0;
			unsigned int _delta = 0;
			unsigned int _total = 0;
			unsigned int _count = 0;
			glm::mat4* _key = nullptr;
			virtual ~animation();
			glm::mat4 interpolation(unsigned int time);
		};
		animation* _anim = nullptr;
		unsigned int _count = 0;
	public:
		virtual ~R3DAnimation() {
			free();
		}
		void free();
		void load(const char* filepath, const R3DBones& bn);
		void load(const char* ptr, size_t size, const R3DBones& bn);
		void update(unsigned int time, const R3DBones& bn) const;
	};
}

