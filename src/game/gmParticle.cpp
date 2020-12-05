#include "gmParticle.h"
#include "../ogCat/sparks.h"

namespace cat
{
namespace spark_effect
{

	class dust final : public cat::spark_effect::sparks
	{
		int _loc_now = -1;
		int _loc_dx = -1;
		int _loc_dz = -1;
		int _loc_ymax = -1;
		int _loc_ymin = -1;
		int _loc_time = -1;
	public:
		void create(const char* filepath, unsigned int count, float visible_range, float y_sky, float y_ground);
		void setHeight(float miny, float maxy) const;
		void move(float x, float z) const;
		virtual void render(const cat::gbuffer& gbuf, unsigned int timeDelta, const cat::camera& cam) override;
	};

	class fireworks final : public cat::spark_effect::sparks
	{
		bool _b_loop = true;

		int _loc_center = -1;
		int _loc_now = -1;
	public:
		void loop(bool opts);
		void create(const char* filepath, unsigned int count, const glm::vec3& center, float radius, float rate);
		void move(const glm::vec3& pos) const;
		virtual void render(const cat::gbuffer& gbuf, unsigned int timeDelta, const cat::camera& cam) override;

	};
}
}

// ==================================================================== //
static const char* dust_vs =
"#version 450								             \n"
"layout (location = 0) in vec3  v_pos;		             \n"
"layout (location = 1) in float v_base;		             \n"
"uniform float u_dx = 0.0f;                              \n"
"uniform float u_dz = 0.0f;                              \n"
"uniform float u_ymax = 10.0f;                           \n"
"uniform float u_ymin = 0.0f;                            \n"
"uniform float u_now = 0.0f;                             \n"
"out vec3  pos;                                          \n"
"out float itn;                                          \n"
"void main()								             \n"
"{											             \n"
"    float r = mod(v_base + u_now, 1.0f);                \n"
"    pos.xz = v_pos.xz + vec2(u_dx, u_dz);               \n"
"    pos.y = r * (u_ymax - u_ymin) + u_ymin;             \n"
"    pos.xz += cos(vec2(pos.y, pos.x));                  \n"
"    itn = sin(r * 3.14159f);                            \n"
"}											             \n"
;

static inline float randf()
{
	const unsigned int precision = 10000;
	return (float)(rand() % (precision + 1)) / (float)(precision);
}
void cat::spark_effect::dust::create(const char* filepath, unsigned int count, float visible_range, float y_sky, float y_ground)
{
	glm::vec3* pos = new glm::vec3[count];
	float* itn = new float[count];
	for (auto i = 0U; i < count; i++) {
		float y_val = randf();
		pos[i].x = (randf() * 2 - 1) * visible_range;
		pos[i].y = y_val;
		pos[i].z = (randf() * 2 - 1) * visible_range;
		itn[i] = 1.0f - y_val;
	}

	_spks.begin(filepath, count);
	_spks.addPos(nullptr, count * sizeof(glm::vec3));
	_spks.addInt(nullptr, count * sizeof(float));
	_spks.end();

	_ctrl.begin(dust_vs);
	_ctrl.addAttribute(pos, count * sizeof(glm::vec3), 3, cat::VERTEX_TYPE::VT_FLOAT);
	_ctrl.addAttribute(itn, count * sizeof(float), 1, cat::VERTEX_TYPE::VT_FLOAT);
	_ctrl.end();
	_loc_dx = _ctrl.shader().getlocation("u_dx");
	_loc_dz = _ctrl.shader().getlocation("u_dz");
	_loc_ymax = _ctrl.shader().getlocation("u_ymax");
	_loc_ymin = _ctrl.shader().getlocation("u_ymin");
	_loc_now = _ctrl.shader().getlocation("u_now");

	setHeight(y_ground, y_sky);

	delete[] pos;
	delete[] itn;
}

void cat::spark_effect::dust::setHeight(float miny, float maxy) const
{
	_ctrl.shader().setfloat(_loc_ymax, maxy);
	_ctrl.shader().setfloat(_loc_ymin, miny);
}

void cat::spark_effect::dust::move(float x, float z) const
{
	_ctrl.shader().setfloat(_loc_dx, x);
	_ctrl.shader().setfloat(_loc_dz, z);
}

void cat::spark_effect::dust::render(const cat::gbuffer& gbuf, unsigned int timeDelta, const cat::camera& cam)
{
	_undateNow(timeDelta);
	_ctrl.shader().setfloat(_loc_now, _now);
	_ctrl.update(_spks);
	_spks.render(gbuf, cam);
}



// ==================================================================== //
static const char* fireworks_vs =
"#version 450								             \n"
"layout (location = 0) in vec3  v_dir;		             \n"
"layout (location = 1) in float v_base;		             \n"
"uniform vec3  u_center = vec3(0.0f);                    \n"
"uniform float u_now = 0.0f;                             \n"
"out vec3  pos;                                          \n"
"out float itn;                                          \n"
"void main()								             \n"
"{											             \n"
"    float r = min(v_base + u_now, 1.0f);                \n"
"    pos = r * v_dir + u_center;                         \n"
"    itn = sin(r * 3.14159f);                            \n"
"}											             \n"
;
void cat::spark_effect::fireworks::loop(bool opts)
{
	if (_b_loop != opts) {
		_b_loop = opts;
		if (opts == true) {
			_now = 0;
		}
	}
}
void cat::spark_effect::fireworks::create(const char* filepath, unsigned int count, const glm::vec3& center, float radius, float rate)
{
	glm::vec3* dir = new glm::vec3[count];
	float* eps = new float[count];
	for (auto i = 0U; i < count; i++) {
		float the = randf() * 6.28318f;
		float phy = randf() * 6.28318f;
		float val = randf() * rate;
		dir[i].x = cos(the) * cos(phy) * radius;
		dir[i].y = sin(the) * cos(phy) * radius;
		dir[i].z = sin(phy) * radius;
		eps[i] = val;
	}

	_spks.begin(filepath, count);
	_spks.addPos(nullptr, count * sizeof(glm::vec3));
	_spks.addInt(nullptr, count * sizeof(float));
	_spks.end();

	_ctrl.begin(fireworks_vs);
	_ctrl.addAttribute(dir, count * sizeof(glm::vec3), 3, cat::VERTEX_TYPE::VT_FLOAT);
	_ctrl.addAttribute(eps, count * sizeof(float), 1, cat::VERTEX_TYPE::VT_FLOAT);
	_ctrl.end();

	_loc_now = _ctrl.shader().getlocation("u_now");
	_loc_center = _ctrl.shader().getlocation("u_center");

	move(center);

	delete[] dir;
	delete[] eps;
}

void cat::spark_effect::fireworks::move(const glm::vec3& pos) const
{
	_ctrl.shader().setvec3(_loc_center, pos);
}

void cat::spark_effect::fireworks::render(const cat::gbuffer& gbuf, unsigned int timeDelta, const cat::camera& cam)
{
	if (_b_loop)
		_undateNow(timeDelta);
	else {
		_now += _rate * ((float)(timeDelta) / 1000.0f);
		if (_now >= 2.0f) {
			return;
		}
	}
	_ctrl.shader().setfloat(_loc_now, _now);
	_ctrl.update(_spks);
	_spks.render(gbuf, cam);
}



// ==========================================================================================//

void ogm::gmParticle::initialize(cat::spark_effect::sparks* spk)
{
	_weak_ptr = spk;
}

void ogm::gmParticle::destroy()
{
	_weak_ptr = nullptr;
}

void ogm::gmParticle::setIntensity(float val) const
{
	if(_weak_ptr)
		_weak_ptr->setIntensity(val);
}

void ogm::gmParticle::setSize(float val) const
{
	if (_weak_ptr)
		_weak_ptr->setSize(val);
}

void ogm::gmParticle::setRate(float val)
{
	if (_weak_ptr)
		_weak_ptr->setRate(val);
}

void ogm::gmParticle::render(const ogm::gmSurface& suf, const ogm::gmSys& sys)
{
	if (_weak_ptr)
		_weak_ptr->render(*suf.gbufferPointer(), (unsigned int)sys.getTimeDelta(), *sys.cameraPointer());
}

ogm::gmParticle::~gmParticle()
{
	destroy();
}

void ogm::gmDust::createDust(const char* filepath, unsigned int count, float visible_range, float y_sky, float y_ground)
{
	auto ptr = new cat::spark_effect::dust();
	gmParticle::initialize(ptr);
	ptr->create(filepath, count, visible_range, y_sky, y_ground);
	_dust.reset(ptr);
}

void ogm::gmDust::move(float x, float z) const
{
	if (_dust != nullptr)
		_dust->move(x, z);
}

void ogm::gmDust::setHeight(float miny, float maxy) const
{
	if (_dust != nullptr)
		_dust->setHeight(miny, maxy);
}

void ogm::gmFireworks::createFireworks(const char* filepath, unsigned int count, const glm::vec3& center, float radius, float rate)
{
	auto ptr = new cat::spark_effect::fireworks();
	gmParticle::initialize(ptr);
	ptr->create(filepath, count, center, radius, rate);
	_fire.reset(ptr);
}

void ogm::gmFireworks::move(const glm::vec3& pos)
{
	if (_fire != nullptr)
		_fire->move(pos);
}

void ogm::gmFireworks::loop(bool opts)
{
	if (_fire != nullptr)
		_fire->loop(opts);
}
