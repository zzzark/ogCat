/*
@author: zrk
@date:   2020/11/14
*/
#pragma once
#include <memory>
#include "gmSys.h"
#include "glm/glm.hpp"

namespace cat
{
	namespace spark_effect
	{
		class sparks;
		class dust;
		class fireworks;
	}
}

namespace ogm
{
	class gmParticle
	{
	protected:
		unsigned int _count = 0;
		cat::spark_effect::sparks* _weak_ptr;

	public:
		void initialize(cat::spark_effect::sparks* spk);
		void destroy();

		void setIntensity(float val) const;
		void setSize(float val) const;
		void setRate(float val);
		void render(const ogm::gmSurface& suf, const ogm::gmSys& sys);

		virtual ~gmParticle();
	};

	class gmDust : virtual public gmParticle
	{
	protected:
		std::shared_ptr<cat::spark_effect::dust> _dust;
	public:
		void createDust(const char* filepath, unsigned int count, float visible_range, float y_sky, float y_ground);

		void move(float x, float z) const;
		void setHeight(float miny, float maxy) const;
	};

	class gmFireworks : virtual public gmParticle
	{
	protected:
		std::shared_ptr<cat::spark_effect::fireworks> _fire;
	public:
		void createFireworks(const char* filepath, unsigned int count, const glm::vec3& center, float radius, float rate);

		void move(const glm::vec3& pos);
		void loop(bool opts);
	};
}
