/*
@author: zrk
@date:   2020/11/13
*/
#pragma once
#include "particle.h"

namespace cat
{
namespace spark_effect
{
	class sparks
	{
	protected:
		float _now = 0.0f;
		float _rate = 1.0f;

		cat::sparks _spks;
		cat::sparksController _ctrl;

		// _now => _rate <scale> timeDelta => [0, 1]
		void _undateNow(unsigned int timeDelta);
	public:
		void  setIntensity(float val) const;
		void  setSize(float val) const;
		void  setRate(float val);
		virtual void render(const cat::gbuffer& gbuf, const unsigned int timeDelta, const cat::camera& cam) = 0;
		virtual ~sparks() {}
	};
}
}

