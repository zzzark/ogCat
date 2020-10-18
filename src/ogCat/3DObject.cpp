#include "3DObject.h"
#include "dbg.h"
#include <fstream>
#include <string>
#include <iostream>
static const char* _header  = "R3DINFO";
static const char* _dynamic = "DYNAMIC";
static const char* _static  = "STATIC";
static std::istream& operator>>(std::istream& is, glm::mat4& m)
{
	is
		>> m[0][0] >> m[0][1] >> m[0][2] >> m[0][3]
		>> m[1][0] >> m[1][1] >> m[1][2] >> m[1][3]
		>> m[2][0] >> m[2][1] >> m[2][2] >> m[2][3]
		>> m[3][0] >> m[3][1] >> m[3][2] >> m[3][3]
		;
	return is;
}
static std::istream& operator>>(std::istream& is, glm::vec3& v)
{
	is >> v.x >> v.y >> v.z;
	return is;
}
static std::istream& operator>>(std::istream& is, glm::vec4& v)
{
	is >> v.x >> v.y >> v.z >> v.w;
	return is;
}
void cat::tdobj::load(const char* filepath)
{
	free();
	std::ifstream ifs;
	ifs.open(filepath, std::ios::binary);
	if (!ifs) {
		std::cout << "3DObject.h warning: can't open file: " << filepath << std::endl;
		return;
	}
	std::string tmp, cnt;
	ifs >> tmp;
	if (tmp.compare(_header) != 0) {
		std::cout << "3DObject.h warning: not a R3D object file: " << filepath;
		return;
	}
	ifs >> tmp;
	if (tmp.compare(_dynamic) == 0) {
		std::string mdl, bon, ani;
		ifs >> tmp; if (tmp.compare("model") == 0) ifs >> mdl; else return;
		ifs >> tmp; if (tmp.compare("bones") == 0) ifs >> bon; else return;
		ifs >> tmp; if (tmp.compare("anims") == 0) ifs >> ani; else return;
		dynamic_mesh* ms = new dynamic_mesh;
		_dms = ms;
		//ms->load(mdl.c_str(), ani.c_str(), bon.c_str());
		//ms->update(0, glm::mat4(1.0f));
		//@TODO


		ifs >> tmp; if (tmp.compare("texture") == 0) ifs >> tmp; else return;
		_tex.createFromFile(tmp.c_str(), true);
		_tex.setwrap(cat::TEXTURE_WRAP::TW_MIRRORED);

		ifs >> tmp; if (tmp.compare("normal") == 0) ifs >> tmp; else return;
		ifs >> tmp; if (tmp.compare("emm") == 0) ifs >> _mtl.emmisive;   else return;
		ifs >> tmp; if (tmp.compare("dif") == 0) ifs >> _mtl.diffuse;	 else return;
		ifs >> tmp; if (tmp.compare("spe") == 0) ifs >> _mtl.specular;	 else return;
		ifs >> tmp; if (tmp.compare("shi") == 0) ifs >> _mtl.shininess;	 else return;
		ifs >> tmp; if (tmp.compare("matrix") == 0) ifs >> _bnd._mat;	 else return;
		ifs >> tmp; if (tmp.compare("bounding") == 0) ifs >> _bnd._count; else return;
		_bnd._boxes = new tdobj::bounding::box[_bnd._count];
		for (unsigned int i = 0; i < _bnd._count; i++) {
			auto& v = _bnd._boxes[i];
			ifs >> v._base.x;
			ifs >> v._base.y;
			ifs >> v._base.z;
			ifs >> v._xdir;
			ifs >> v._ydir;
			ifs >> v._zdir;
			v._p[0] = v._base;
			v._p[1] = v._base + glm::vec3(v._xdir, 0, 0);
			v._p[2] = v._base + glm::vec3(0, v._ydir, 0);
			v._p[3] = v._base + glm::vec3(0, 0, v._zdir);
			v._p[4] = v._base + glm::vec3(v._xdir, v._ydir, 0);
			v._p[5] = v._base + glm::vec3(v._xdir, 0, v._zdir);
			v._p[6] = v._base + glm::vec3(0, v._ydir, v._zdir);
			v._p[7] = v._base + glm::vec3(v._xdir, v._ydir, v._zdir);
		}
	}
	else if (tmp.compare(_static) == 0) {
		std::string mdl;
		ifs >> tmp; if (tmp.compare("model") == 0) ifs >> mdl;  else return;
		ifs >> tmp; if (tmp.compare("bones") == 0) ifs >> tmp;  else return;
		ifs >> tmp; if (tmp.compare("anims") == 0) ifs >> tmp;  else return;
		static_mesh* ms = new static_mesh;
		_sms = ms;
		ms->load(mdl.c_str());

		ifs >> tmp; if (tmp.compare("texture") == 0) ifs >> tmp; else return;
		_tex.createFromFile(tmp.c_str(), true);
		_tex.setwrap(cat::TEXTURE_WRAP::TW_MIRRORED);

		ifs >> tmp; if (tmp.compare("normal") == 0) ifs >> tmp;  else return;
		ifs >> tmp; if (tmp.compare("emm") == 0) ifs >> _mtl.emmisive;    else return;
		ifs >> tmp; if (tmp.compare("dif") == 0) ifs >> _mtl.diffuse;	  else return;
		ifs >> tmp; if (tmp.compare("spe") == 0) ifs >> _mtl.specular;	  else return;
		ifs >> tmp; if (tmp.compare("shi") == 0) ifs >> _mtl.shininess;	  else return;
		ifs >> tmp; if (tmp.compare("matrix") == 0) ifs >> _bnd._mat;     else return;
		ifs >> tmp; if (tmp.compare("bounding") == 0) ifs >> _bnd._count; else return;
		_bnd._boxes = new tdobj::bounding::box[_bnd._count];
		for (unsigned int i = 0; i < _bnd._count; i++) {
			auto& v = _bnd._boxes[i];
			ifs >> v._base.x;
			ifs >> v._base.y;
			ifs >> v._base.z;
			ifs >> v._xdir;
			ifs >> v._ydir;
			ifs >> v._zdir;
			v._p[0] = v._base;
			v._p[1] = v._base + glm::vec3(v._xdir, 0, 0);
			v._p[2] = v._base + glm::vec3(0, v._ydir, 0);
			v._p[3] = v._base + glm::vec3(0, 0, v._zdir);
			v._p[4] = v._base + glm::vec3(v._xdir, v._ydir, 0);
			v._p[5] = v._base + glm::vec3(v._xdir, 0, v._zdir);
			v._p[6] = v._base + glm::vec3(0, v._ydir, v._zdir);
			v._p[7] = v._base + glm::vec3(v._xdir, v._ydir, v._zdir);
		}
	}
	else {
		std::cout << "3DObject.h warning: can't load R3D object file: " << filepath << "  type: " << tmp;
		return;
	}
}

void cat::tdobj::draw()
{
//@TODO
}

void cat::tdobj::free()
{
	delete _sms; _sms = nullptr;
	delete _dms; _dms = nullptr;
	_tex.release();
}

cat::tdobj::~tdobj()
{
	free();
}

void cat::tdobj::move(float dx, float dy, float dz)
{
	_bnd._mat[3][0] += dx;
	_bnd._mat[3][1] += dy;
	_bnd._mat[3][2] += dz;
}

void cat::tdobj::moveAbs(float x, float y, float z)
{
	_bnd._mat[3][0] = x;
	_bnd._mat[3][1] = y;
	_bnd._mat[3][2] = z;
}

void cat::tdobj::setMat(const glm::mat4& m)
{
	_bnd._mat = m;
}


//=====================================================================================================//


bool cat::tdobj::bounding::collide(const bounding& st) const
{
	glm::vec4 d;
	for (unsigned int i = 0; i < _count; i++) {
		for (unsigned int j = 0; j < st._count; j++) {
			auto b = st._mat * glm::vec4(st._boxes[j]._base, 1.0f);
			for (unsigned int k = 0; k < 8; k++) {
				auto p = _mat * glm::vec4(_boxes[i]._p[k], 1.0f);
				auto& x = st._boxes[j]._xdir;
				auto& y = st._boxes[j]._ydir;
				auto& z = st._boxes[j]._zdir;
				d = p - b;
				if (0 <= d.x && d.x <= x)
					if (0 <= d.y && d.y <= y)
						if (0 <= d.z && d.z <= z)
							return true;
			}
		}
	}
	return false;
}

cat::tdobj::bounding::~bounding()
{
	delete[] _boxes; _boxes = nullptr;
	_count = 0;
}