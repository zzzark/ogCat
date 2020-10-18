#pragma once
#include "frameEffect.h"
#include "frameBuffer.h"

namespace cat
{
	class effectBlur
	{
		frameEffect _eff_v;
		frameEffect _eff_h;		
		frameBuffer _tmpBuffer;
	public:
		// 
		// 
		void create(unsigned int width, unsigned int height);

		// apply effect to destBuffer, if nullptr == destBuffer then render to screen
		//  tex -> tmpBuffer -> destBuffer
		//
		void applyEffect(const frameBuffer* destBuffer, const texture2D& originalTex);

	};
}