/*
@author:  zrk
@date:    2020/6/19    
example:
  shadowBuffer s;
  s.create(W, H, 16U, 0.5f);
  s.begin();
  {
	  s.begin_shadow();
	  ...(accumulation)
	  s.end_shadow();

	  s.begin_shadow();
	  ...(accumulation)
	  s.end_shadow();
  }
  s.end();
  s.getTex();

*/

#pragma once
#include "frameEffect.h"
#include "frameBuffer.h"
#include "gbuffer.h"
namespace cat
{
	class shadowBuffer final
	{
		unsigned int _next_bind;
		frameEffect _eff_v;
		frameEffect _eff_h;
		frameBuffer _sha;
	public:
		shadowBuffer();
		void create(unsigned int width, unsigned int height, unsigned int bit = 16U, float scaleFactor = 0.5f);
		void applyEffect(const gbuffer& depth) const;
		
		// w -> factor * w
		// h -> factor * h
		// the smaller, the blurier
		// the default factor is 0.5
		//
		void setScaleFactor(float factor);


		void begin();
		void end();
		void begin_shadow();
		void end_shadow();
		const texture2D& getTex() const;


		inline void release() { _sha.release(); _eff_v.release(); _eff_h.release(); }
	};
}