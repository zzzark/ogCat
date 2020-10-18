#pragma once
#include "frameEffect.h"

namespace cat
{
	class effectFXAA : protected frameEffect
	{
		// used to identify if the input texture is changed
		const texture2D* _lastTex;
	public:
		effectFXAA();

		// create effect in constructor, effectFXAA obj(true); <==> effectFXAA obj;
		//
		effectFXAA(bool autoCreate);

		// 
		// 
		void create();

		// apply effect to currently bound draw framebuffer
		//  e.g.
		//  renderer::BeginFrame( ... );
		//  obj.applyEffect( ... );
		//  renderer::EndFrame();
		void applyEffect(const texture2D& originalTex);

	};
}