#include "sparks.h"
#include <stdlib.h>

// ==================================================================== //

void cat::spark_effect::sparks::setIntensity(float val) const
{
	_spks.setIntensity(val);
}

void cat::spark_effect::sparks::setSize(float val) const
{
	_spks.setSize(val);
}

void cat::spark_effect::sparks::setRate(float val)
{
	_rate = val;
}

void cat::spark_effect::sparks::_undateNow(unsigned int timeDelta)
{
	_now += _rate * ((float)(timeDelta) / 1000.0f);
	_now = fmod(_now, 1.0f);
}

