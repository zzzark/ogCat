/*
@author: zrk
@date:   2020/11/6
*/
#pragma once
#include "glm/glm.hpp"

namespace cat
{
	class meshInstance;
	class R3DBones;
	class R3DAnimation;
};

namespace ogm
{
	class gmObj final
	{
	private:
		cat::meshInstance* _inst = nullptr;
		cat::R3DBones* _bon = nullptr;
		cat::R3DAnimation* _ani = nullptr;
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
		enum class GMOBJ_ERR {
			ERR_LOADING,	// error(s) occur while loading the file
			ERR_HEADER,		// not a "R3DINFO" file
			ERR_TYPE,		// not a "STATIC" or "DYNAMIC" file
			ERR_FORMAT,		// wrong format
			ERR_PARAMETER,	// illegal parameter
		};
	public:
		GMOBJ_ERR createFromFile(const char* filepath);
		~gmObj();
	};
	class gmWorld
	{
	protected:
		//cat::skybox* _sky = nullptr;
		//cat::terrainBlock* _ground = nullptr;
	public:
	};
}