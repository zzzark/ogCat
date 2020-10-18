#pragma once
#include "frameBuffer.h"
#include "frameEffect.h"

namespace cat
{
	class lightEffect
	{
	public:
		virtual void addLight() const = 0 {};
	};
	class directionalLight final : public lightEffect
	{

	};
	class pointLight final : public lightEffect
	{

	};
	class directionalLightPBR final : public lightEffect
	{

	};
	class lightBuffer final
	{
		unsigned int _current_index;
		frameBuffer _lit;
	public:
		lightBuffer() { _current_index = 0; }
		void create(unsigned int width, unsigned int height, unsigned int channel = 3U, unsigned int bit = 16U);

		// clear the ##light buffer## and the ##deputy light buffer##
		//
		void begin() const;

		// apply effect, accumulation light buffer will be bind to slot 0
		//
		void applyEffect(const lightEffect& eff) const;

		// finish rendering
		//
		void end() const;


	};
}