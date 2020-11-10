#include <fstream>
#include "gmObject.h"
#include "mesh_instance.h"
#include "R3DLoader.h"

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
	delete _inst; _inst = nullptr;
	delete _bon ;  _bon = nullptr;
	delete _ani ;  _ani = nullptr;  
	_inst = new cat::meshInstance;

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
		_bon->load(fbon.c_str());
		_ani->load(fani.c_str(), *_bon);
		_inst->loadDynamicMesh(fbin.c_str(), *_bon);
	}
	// texture
	if (read2String(ifs, sa, sb) == false) return GMOBJ_ERR::ERR_FORMAT;
	_inst->loadTexture(sb.c_str());
	
	// normal
	if (read2String(ifs, sa, sb) == false) return GMOBJ_ERR::ERR_FORMAT;

	ifs >> sa;
	ifs >> _inst->mtl.emmisive;

	ifs >> sa;
	ifs >> _inst->mtl.diffuse;

	ifs >> sa;
	ifs >> _inst->mtl.specular;

	ifs >> sa;
	ifs >> _inst->mtl.shininess;

	ifs >> sa;
	ifs >> _inst->mdl;
}

ogm::gmObj::~gmObj()
{
	delete _inst; _inst = nullptr;
	delete _bon ;  _bon = nullptr;
	delete _ani ;  _ani = nullptr;
}
