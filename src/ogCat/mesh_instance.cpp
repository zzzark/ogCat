#include <string>
#include <fstream>
#include "mesh_instance.h"
#include "mesh.h"
#include "dynamic_mesh.h"

const char* cat::meshInstance::_s_header  = "R3DINFO";
const char* cat::meshInstance::_s_static  = "STATIC";
const char* cat::meshInstance::_s_dynamic = "DYNAMIC";
const char* cat::meshInstance::_s_model = "model";
const char* cat::meshInstance::_s_bones = "bones";
const char* cat::meshInstance::_s_anims = "anims";
const char* cat::meshInstance::_s_texture = "texture";
const char* cat::meshInstance::_s_normal = "normal";
const char* cat::meshInstance::_s_emm = "emm";
const char* cat::meshInstance::_s_dif = "dif";
const char* cat::meshInstance::_s_spe = "spe";
const char* cat::meshInstance::_s_shi = "shi";
const char* cat::meshInstance::_s_matrix = "matrix";
const char* cat::meshInstance::_s_bounding = "bounding";


bool cat::meshInstance::loadStaticMesh(const char* filepath)
{
	_sms = nullptr;
	_dms = nullptr;
	delete _ms; _ms = nullptr;
	_sms = new static_mesh;
	_sms->load(filepath);
	_ms = _sms;
	return _ms != nullptr;
}

bool cat::meshInstance::loadDynamicMesh(const char* filepath, R3DBones& bones)
{
	_sms = nullptr;
	_dms = nullptr;
	delete _ms; _ms = nullptr;
	_dms = new dynamic_mesh;
	_dms->load(filepath, bones);
	_ms = _dms;
	return _ms != nullptr;
}

bool cat::meshInstance::loadTexture(const char* filepath)
{
	_tex.createFromFile(filepath, true);
	return _tex.getID() != 0;
}

cat::meshInstance::~meshInstance()
{
	_sms = nullptr;
	_dms = nullptr;
	delete _ms; _ms = nullptr;
}

void cat::meshInstance::move(float dx, float dy, float dz)
{
	mdl[3][0] += dx;
	mdl[3][1] += dy;
	mdl[3][2] += dz;
}

void cat::meshInstance::moveTo(float x, float y, float z)
{
	mdl[3][0] = x;
	mdl[3][1] = y;
	mdl[3][2] = z;
}

void cat::meshInstance::moveTo(const glm::vec3& pos)
{
	mdl[3][0] = pos.x;
	mdl[3][1] = pos.y;
	mdl[3][2] = pos.z;
}
