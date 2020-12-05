/*
@author:	zrk
@date:		2020/6/28
*/
#pragma once
#include "frameBuffer.h"
#include "frameEffect.h"
#include "material.h"
namespace cat
{
	class gbuffer final
	{
		frameBuffer _gbuffer;

		unsigned int _width  = 0;
		unsigned int _height = 0;
	public:
		static const unsigned int MAX_BUFFER_LAYER_COUNT = 7;
		enum BUFFER_LAYER {
			NORMAL       = 0,
			COLOR        = 1,
			DIFFUSE      = 2,
			SPECULAR     = 3,
			RECONG       = 4,
			EMMISIVE     = 5,
			POSITION     = 6,  /* @2020/11/14: fix bugs of rendering sparks */
		};
		// 0: normal
		// 1: color
		// 2: diffuse
		// 3: specular + shininess
		// 4: recongnization
		// 5: emmisive
		// 6: position
		//
		void create(unsigned int width, unsigned int height);

		const glm::vec3 getPixel(unsigned int index, float x, float y) const;
		const glm::vec3 getRecognizationPixel(float x, float y) const;
		const  void switchBuffers(int so, int s1 = -1, int s2 = -1, int s3 = -1, int s4 = -1, int s5 = -1, int s6 = -1, int s7 = -1) const;
		inline void switchBuffers(unsigned int count) const { _gbuffer.switchBuffers(count); }
		inline void switchBuffer(unsigned int index) const { _gbuffer.switchBuffer(index); }
		inline void switchNml() const { switchBuffer(BUFFER_LAYER::NORMAL   ); }
		inline void switchCol() const { switchBuffer(BUFFER_LAYER::COLOR    ); }
		inline void switchDif() const { switchBuffer(BUFFER_LAYER::DIFFUSE  ); }
		inline void switchSpe() const { switchBuffer(BUFFER_LAYER::SPECULAR ); }
		inline void switchReg() const { switchBuffer(BUFFER_LAYER::RECONG   ); }
		inline void switchEmm() const { switchBuffer(BUFFER_LAYER::EMMISIVE ); }
		inline void switchPos() const { switchBuffer(BUFFER_LAYER::POSITION ); }
		inline unsigned int getWidth() const { return _width; }
		inline unsigned int getHeight() const { return _height; }
		inline const texture2D& nml_tex() const { return _gbuffer[BUFFER_LAYER::NORMAL   ]; }
		inline const texture2D& col_tex() const { return _gbuffer[BUFFER_LAYER::COLOR    ]; }
		inline const texture2D& dif_tex() const { return _gbuffer[BUFFER_LAYER::DIFFUSE  ]; }
		inline const texture2D& spe_tex() const { return _gbuffer[BUFFER_LAYER::SPECULAR ]; }
		inline const texture2D& reg_tex() const { return _gbuffer[BUFFER_LAYER::RECONG   ]; }
		inline const texture2D& emm_tex() const { return _gbuffer[BUFFER_LAYER::EMMISIVE ]; }
		inline const texture2D& pos_tex() const { return _gbuffer[BUFFER_LAYER::POSITION ]; }
		inline const texture2D& dep_tex() const { return _gbuffer.getDep(); }

		inline void bind()   const { _gbuffer.bindForBoth(); }
		inline void unbind() const { _gbuffer.unbindForBoth(); }
	};
}