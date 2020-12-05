/*
@author: zrk
@date:   2020/11/6
*/
#pragma once
#include "glm/glm.hpp"
#include "gmRenderer.h"
#include "../collision/basicBlock.h"

#include <memory>

namespace cat
{
	class meshInstance;
	class R3DBones;
	class R3DAnimation;
};

namespace ogm
{
	class gmObj final : protected collision::basicBlock
	{
	public:
		enum class GMOBJ_ERR {
			ERR_LOADING,	// error(s) occur while loading the file
			ERR_HEADER,		// not a "R3DINFO" file
			ERR_TYPE,		// not a "STATIC" or "DYNAMIC" file
			ERR_FORMAT,		// not supported format
			ERR_PARAMETER,	// illegal parameter(s)
			ERR_NO_ERROR,	// no error
		};
		enum class GMOBJ_DYNAMIC_STATE {
			ST_LOOP,
			ST_PAUSED,
			ST_STOPPED,
		};
	private:
		std::shared_ptr<cat::meshInstance> _inst;
		std::shared_ptr<cat::R3DBones> _bon;
		std::shared_ptr<cat::R3DAnimation> _ani;

		GMOBJ_DYNAMIC_STATE _state = GMOBJ_DYNAMIC_STATE::ST_LOOP;
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
		GMOBJ_ERR createFromFile(const char* filepath);
		gmObj() {}
		~gmObj();

		void loop();
		void pause();
		void stop();
		void scale(float factor);
		void move(float dx, float dy, float dz);
		void moveTo(float x, float y, float z);
		void moveTo(const glm::vec3& pos);

		glm::mat4& get_model_matrix();

		GMOBJ_DYNAMIC_STATE getState() { return _state; }

		cat::meshInstance* meshInstancePointer() { return _inst.get(); }
		cat::R3DBones* bonesPointer() { return _bon.get(); }
		cat::R3DAnimation* animPointer() { return _ani.get(); }

		const cat::meshInstance* meshInstancePointer() const { return _inst.get(); }
		const cat::R3DBones* bonesPointer() const { return _bon.get(); }
		const cat::R3DAnimation* animPointer() const { return _ani.get(); }
	};
}