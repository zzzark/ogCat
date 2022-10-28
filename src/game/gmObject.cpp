#include <fstream>
#include "gmObject.h"
#include "ogCat/dynamic_mesh.h"
#include "ogCat/mesh_instance.h"
#include "ogCat/R3DLoader.h"


const char* ogm::gmObj::_s_header   = "R3DINFO";
const char* ogm::gmObj::_s_static   = "STATIC";
const char* ogm::gmObj::_s_dynamic  = "DYNAMIC";
const char* ogm::gmObj::_s_model    = "model";
const char* ogm::gmObj::_s_bones    = "bones";
const char* ogm::gmObj::_s_anims    = "anims";
const char* ogm::gmObj::_s_texture  = "texture";
const char* ogm::gmObj::_s_normal   = "normal";
const char* ogm::gmObj::_s_emm      = "emm";
const char* ogm::gmObj::_s_dif      = "dif";
const char* ogm::gmObj::_s_spe      = "spe";
const char* ogm::gmObj::_s_shi      = "shi";
const char* ogm::gmObj::_s_matrix   = "matrix";
const char* ogm::gmObj::_s_bounding = "bounding";

static std::istream& operator>>(std::istream& is, glm::vec3& v)
{
	is >> v[0] >> v[1] >> v[2];
	return is;
}

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


static bool read2String(std::ifstream& ifs, std::string& a, std::string& b)
{
	if (!ifs.eof()) ifs >> a; else return false;
	if (!ifs.eof()) ifs >> b; else return false;
	return true;
}

static std::ostream& operator<< (std::ostream& os, const glm::mat4& m)
{
	os
		<< m[0][0] << ' ' << m[0][1] << ' ' << m[0][2] << ' ' << m[0][3] << ' '
		<< m[1][0] << ' ' << m[1][1] << ' ' << m[1][2] << ' ' << m[1][3] << ' '
		<< m[2][0] << ' ' << m[2][1] << ' ' << m[2][2] << ' ' << m[2][3] << ' '
		<< m[3][0] << ' ' << m[3][1] << ' ' << m[3][2] << ' ' << m[3][3]
		;
	return os;
}

ogm::gmObj::GMOBJ_ERR ogm::gmObj::createFromFile(const char* filepath)
{
	_inst = nullptr;
	 _bon = nullptr;
	 _ani = nullptr;
	_inst.reset(new cat::meshInstance);

	std::string sa, sb;
	std::ifstream ifs(filepath);
	if (!ifs)
		return GMOBJ_ERR::ERR_LOADING;
	if (read2String(ifs, sa, sb) == false) return GMOBJ_ERR::ERR_FORMAT;
	if (sa != _s_header) return GMOBJ_ERR::ERR_HEADER;
	if (sb != _s_static && sb != _s_dynamic) return GMOBJ_ERR::ERR_TYPE;
	std::string bin, fbin, bon, fbon, ani, fani;
	if  (read2String(ifs, bin, fbin) == false ||
		 read2String(ifs, bon, fbon) == false || 
		 read2String(ifs, ani, fani) == false)
		return GMOBJ_ERR::ERR_FORMAT;
	if (sb == _s_static)
		_inst->loadStaticMesh(fbin.c_str());
	else {
		_bon.reset(new cat::R3DBones);
		_ani.reset(new cat::R3DAnimation);
		_bon->load(fbon.c_str());
		_ani->load(fani.c_str(), *_bon);
		_inst->loadDynamicMesh(fbin.c_str(), *_bon);
	}
	// texture
	if (read2String(ifs, sa, sb) == false) return GMOBJ_ERR::ERR_FORMAT;
	_inst->loadTexture(sb.c_str());
	
	// normal
	if (read2String(ifs, sa, sb) == false) return GMOBJ_ERR::ERR_FORMAT;

	// emm
	ifs >> sa;
	ifs >> _inst->mtl.emmisive;

	// div
	ifs >> sa;
	ifs >> _inst->mtl.diffuse;

	// spe
	ifs >> sa;
	ifs >> _inst->mtl.specular;

	// shi
	ifs >> sa;
	ifs >> _inst->mtl.shininess;

	// matrix
	ifs >> sa;
	ifs >> _inst->mdl;

	// bounding
	ifs >> sa;
	int bounding_count = 0;
	ifs >> bounding_count;
	if (bounding_count != 0) {
		glm::vec3 pos;
		float x, y, z;
		ifs >> pos.x >> pos.y >> pos.z;
		ifs >> x >> y >> z;
		collision::basicBlock::set(pos, x, y, z);
	}
	collision::basicBlock::update(_inst->mdl);

	return GMOBJ_ERR::ERR_NO_ERROR;
}

ogm::gmObj::~gmObj()
{
	_inst = nullptr;
	 _bon = nullptr;
	 _ani = nullptr;
}

void ogm::gmObj::loop()
{
	_state = GMOBJ_DYNAMIC_STATE::ST_LOOP;
	if (_inst && _bon && _ani && _inst->getDynamicMesh())
		_inst->getDynamicMesh()->loop();
}

void ogm::gmObj::pause()
{
	_state = GMOBJ_DYNAMIC_STATE::ST_PAUSED;
	if (_inst && _bon && _ani && _inst->getDynamicMesh())
		_inst->getDynamicMesh()->pause();
}

void ogm::gmObj::stop()
{
	_state = GMOBJ_DYNAMIC_STATE::ST_STOPPED;
	if (_inst && _bon && _ani && _inst->getDynamicMesh())
		_inst->getDynamicMesh()->stop(*_ani);
}

void ogm::gmObj::scale(float factor)
{
	if (_inst) {
		_inst->mdl[0][0] *= factor;
		_inst->mdl[1][1] *= factor;
		_inst->mdl[2][2] *= factor;
		collision::basicBlock::update(_inst->mdl);
	}
}

void ogm::gmObj::move(float dx, float dy, float dz)
{
	if (_inst) {
		_inst->move(dx, dy, dz);
		collision::basicBlock::update(_inst->mdl);
	}
}

void ogm::gmObj::moveTo(float x, float y, float z)
{
	if (_inst) {
		_inst->moveTo(x, y, z);
		collision::basicBlock::update(_inst->mdl);
	}
}

void ogm::gmObj::moveTo(const glm::vec3& pos)
{
	if (_inst) {
		_inst->moveTo(pos);
		collision::basicBlock::update(_inst->mdl);
	}
}

glm::mat4& ogm::gmObj::get_model_matrix()
{
	static glm::mat4 identity(1);
	if (_inst)
		return _inst->mdl;
	else
		return identity;
}
