#include "R3DLoader.h"
#include <fstream>
#include <cstring>
#include <iostream>

static const char R3D_HEADER_STATIC[] =    { "R3D_STATIC" };
static const char R3D_HEADER_DYNAMI[] =    { "R3D_DYNAMI" };
static const char R3D_HEADER_HIERARCHY[] = { " HIERARCHY" };
static const char R3D_HEADER_ANIMATION[] = { " ANIMATION" };
static constexpr unsigned int HEADER_SIZE = sizeof(R3D_HEADER_STATIC) - 1;


cat::R3DLoader::R3DLoader()
{
	_xyz = nullptr;
	_uv = nullptr;
	_str = nullptr;
	_b = nullptr;
	_w = nullptr;
	_ids = nullptr;
	_idsCnt = 0;
	_vtsCnt = 0;
}
cat::R3DLoader::~R3DLoader()
{
	free();
}
void cat::R3DLoader::load(const char* filepath)
{
	free();
	std::ifstream ifs;
	ifs.open(filepath, std::ios::binary);
	if (!ifs) {
		std::cout << "R3DLoader.h(cat::R3DLoader::load(const char* filepath)) warning: can't open file: " << filepath << std::endl;
		return;
	}
	char header[HEADER_SIZE + 1] = { 0 };
	ifs.read(header, HEADER_SIZE);
	if (strcmp(R3D_HEADER_STATIC, header) == 0) {
		ifs.read((char*)&_vtsCnt, sizeof(_vtsCnt));
		ifs.read((char*)&_idsCnt, sizeof(_idsCnt));
		if (_vtsCnt <= 0 || _idsCnt <= 0) {
			std::cout << "R3DLoader.h(cat::R3DLoader::load(const char* filepath)) warning: R3D file corrupted: " << filepath << std::endl;
			ifs.close();
			return;
		}
		auto nowP = ifs.tellg();
		ifs.seekg(0, std::ios::end);
		auto endP = ifs.tellg();
		auto detP = endP - nowP;
		ifs.seekg(nowP);
		/*
		x y z
		u v
		s t r
		i
		*/
		if (detP != (std::streamoff)_vtsCnt * 8 * sizeof(float) +
					(std::streamoff)_idsCnt * sizeof(unsigned int)) {
			std::cout << "R3DLoader.h(cat::R3DLoader::load(const char* filepath)) warning: R3D file corrupted: " << filepath << std::endl;
			ifs.close();
			return;
		}
		_xyz = new float[(size_t)_vtsCnt * 3];
		_uv  = new float[(size_t)_vtsCnt * 2];
		_str = new float[(size_t)_vtsCnt * 3];
		_ids = new unsigned int[(size_t)_idsCnt];
		ifs.read((char*)_xyz, (size_t)_vtsCnt * 3 * sizeof(float));
		ifs.read((char*)_uv,  (size_t)_vtsCnt * 2 * sizeof(float));
		ifs.read((char*)_str, (size_t)_vtsCnt * 3 * sizeof(float));
		ifs.read((char*)_ids, (size_t)_idsCnt * sizeof(unsigned int));
	}
	else if (strcmp(R3D_HEADER_DYNAMI, header) == 0) {
		ifs.read((char*)&_vtsCnt, sizeof(_vtsCnt));
		ifs.read((char*)&_idsCnt, sizeof(_idsCnt));
		if (_vtsCnt <= 0 || _idsCnt <= 0) {
			std::cout << "R3DLoader.h(cat::R3DLoader::load(const char* filepath)) warning: R3D file corrupted: " << filepath << std::endl;
			ifs.close();
			return;
		}
		auto nowP = ifs.tellg();
		ifs.seekg(0, std::ios::end);
		auto endP = ifs.tellg();
		auto detP = endP - nowP;
		ifs.seekg(nowP);
		/*
		x y z
		u v
		s t r
		w w w w
		b b b b
		i
		*/
		if (detP != (std::streamoff)_vtsCnt * 12 * sizeof(float) + 
					(std::streamoff)_vtsCnt * 4 * sizeof(unsigned int)+
					(std::streamoff)_idsCnt * sizeof(unsigned int)) {
			std::cout << "R3DLoader.h(cat::R3DLoader::load(const char* filepath)) warning: R3D file corrupted: " << filepath << std::endl;
			ifs.close();
			return;
		}
		_xyz = new float[(size_t)_vtsCnt * 3];
		_uv  = new float[(size_t)_vtsCnt * 2];
		_str = new float[(size_t)_vtsCnt * 3];
		_w   = new float[(size_t)_vtsCnt * 4];
		_b   = new unsigned int[(size_t)_vtsCnt * 4];
		_ids = new unsigned int[(size_t)_idsCnt];
		ifs.read((char*)_xyz, (size_t)_vtsCnt * 3 * sizeof(float));
		ifs.read((char*)_uv,  (size_t)_vtsCnt * 2 * sizeof(float));
		ifs.read((char*)_str, (size_t)_vtsCnt * 3 * sizeof(float));
		ifs.read((char*)_w,   (size_t)_vtsCnt * 4 * sizeof(float));
		ifs.read((char*)_b,   (size_t)_vtsCnt * 4 * sizeof(unsigned int));
		ifs.read((char*)_ids, (size_t)_idsCnt * sizeof(unsigned int));
	}
	else {
		std::cout << "R3DLoader.h(cat::R3DLoader::load(const char* filepath)) warning: not a R3D file: " << filepath << std::endl;
		ifs.close();
		return;
	}
	ifs.close();
}

void cat::R3DLoader::load(const char* ptr, size_t size)
{
}

void cat::R3DLoader::free()
{
	delete[] _xyz;	_xyz = nullptr;
	delete[] _uv;	_uv = nullptr;
	delete[] _str;	_str = nullptr;
	delete[] _b;	_b = nullptr;
	delete[] _w;	_w = nullptr;
	delete[] _ids;	_ids = nullptr;
	_idsCnt = 0;
	_vtsCnt = 0;
}


//=======================================================================================================//
class cat::R3DBones::bone {
public:
	std::string _name;
	bone* _children = nullptr;
	unsigned int _count = 0;
	unsigned int _index = 0;
	~bone() {
		delete[] _children;
		_count = 0;
		_children = nullptr;
	}
};
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
static void __read(std::ifstream& ifs, cat::R3DBones::bone* ptr)
{
	ifs >> ptr->_name;
	ifs >> ptr->_count;
	ptr->_children = new cat::R3DBones::bone[ptr->_count];
	for (unsigned int i = 0; i < ptr->_count; i++) {
		__read(ifs, &ptr->_children[i]);
	}
}
static cat::R3DBones::bone* __search(const char* name, cat::R3DBones::bone* ptr)
{
	if (ptr->_name.compare(name) == 0) {
		return ptr;
	}
	for (unsigned int i = 0; i < ptr->_count; i++) {
		auto ret = __search(name, &ptr->_children[i]);
		if (ret) return ret;
	}
	return nullptr;
}
static void __update(cat::R3DBones::bone* ptr, const glm::mat4& m, glm::mat4* _cmb, const glm::mat4* _trs)
{
	unsigned int i = ptr->_index;
	_cmb[i] = m * _trs[i];
	for (unsigned int j = 0; j < ptr->_count; j++) {
		__update(&ptr->_children[j], _cmb[i], _cmb, _trs);
	}
}
void cat::R3DBones::load(const char* filepath)
{
	free();
	std::ifstream ifs;
	ifs.open(filepath, std::ios::binary);
	if (!ifs) {
		std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: can't open file: " << filepath << std::endl;
		return;
	}
	char header[HEADER_SIZE + 1] = { 0 };
	ifs.read(header, HEADER_SIZE);
	if (strcmp(R3D_HEADER_DYNAMI, header) != 0) {
		ifs.close();
		std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: not a R3D file: " << filepath << std::endl;
		return;
	}
	ifs.read(header, HEADER_SIZE);
	if (strcmp(R3D_HEADER_HIERARCHY, header) != 0) {
		ifs.close();
		std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: not a R3D file: " << filepath << std::endl;
		return;
	}
	_root = new bone;
	__read(ifs, _root);
	ifs >> _count;
	if (_count >= 1024) {
		ifs.close();
		delete   _root; _root = nullptr;
		std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: too much bones: " << filepath << std::endl;
		return;
	}
	_ori = new glm::mat4[_count];
	_trs = new glm::mat4[_count];
	_off = new glm::mat4[_count];
	_cmb = new glm::mat4[_count];
	std::string str;
	for (unsigned int i = 0; i < _count; i++) {
		ifs >> str;
		ifs >> _trs[i];
		ifs >> _off[i];
		_ori[i] = _trs[i];
		auto ret = __search(str.c_str(), _root);
		if (ret != nullptr) {
			ret->_index = i;
		}
	}
	ifs.close();
}

void cat::R3DBones::load(const char* ptr, size_t size)
{
}

void cat::R3DBones::update(const glm::mat4& m)
{
	__update(_root, m, _cmb, _trs);
	for (unsigned int i = 0; i < _count; i++) {
		_cmb[i] = _cmb[i] * _off[i];
	}
}

void cat::R3DBones::reset()
{
	for (unsigned int i = 0; i < _count; i++) {
		_trs[i] = _ori[i];
	}
}

int cat::R3DBones::getIndex(const char* name) const
{
	auto ret = __search(name, _root);
	if (ret == nullptr)
		return -1;
	return ret->_index;
}

void cat::R3DBones::free()
{
	delete[] _ori;  _ori = nullptr;
	delete[] _trs;  _trs = nullptr;
	delete[] _off;  _off = nullptr;
	delete[] _cmb;  _cmb = nullptr;
	delete   _root; _root = nullptr;
	_count = 0;
}
//=======================================================================================================//

cat::R3DAnimation::animation::~animation()
{
	delete[] _key; _key = nullptr;
	_index = 0;
	_delta = 0;
	_total = 0;
	_count = 0;
}

glm::mat4 cat::R3DAnimation::animation::interpolation(unsigned int time)
{
	if(_count == 0)
		return glm::mat4(1.0f);
	if (_count == 1 || _delta == 0 || _total == 0)
		return _key[0];
	time = time % _total;
	// n * det + r = time
	// time / det = n ... r
	unsigned int n = time / _delta;
	unsigned int r = time - _delta * n;
	unsigned int pre, nex; // previous, next
	pre = n;
	nex = n + 1;
	if (n >= _count - 1) {
		pre = 0;
		nex = 1;
	}
	float rate = (float)r / (float)_delta;
	return _key[pre] * (1.0f - rate) + _key[nex] * rate;
}

void cat::R3DAnimation::free()
{
	delete[] _anim; _anim = nullptr;
	_count = 0;
}

void cat::R3DAnimation::load(const char* filepath, const R3DBones& bn)
{
	free();
	std::ifstream ifs;
	ifs.open(filepath, std::ios::binary);
	if (!ifs) {
		std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: can't open file: " << filepath << std::endl;
		return;
	}
	char header[HEADER_SIZE + 1] = { 0 };
	ifs.read(header, HEADER_SIZE);
	if (strcmp(R3D_HEADER_DYNAMI, header) != 0) {
		ifs.close();
		std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: not a R3D file: " << filepath << std::endl;
		return;
	}
	ifs.read(header, HEADER_SIZE);
	if (strcmp(R3D_HEADER_ANIMATION, header) != 0) {
		ifs.close();
		std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: not a R3D file: " << filepath << std::endl;
		return;
	}
	ifs >> _count;
	if (_count == 0 || _count > 256)
		return;
	unsigned int counter = 0;
	_anim = new animation[_count];
	std::string name;
	counter = 1;
	while (ifs.eof() == false && ifs >> name && counter <= _count) {
		unsigned int i = bn.getIndex(name.c_str());
		if (i < 0) {
			std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: can't find such bone: " << name << " file:" << filepath << std::endl;
			return;
		}
		auto& a = _anim[counter - 1];
		a._index = i;
		ifs >> a._delta;
		ifs >> a._total;
		ifs >> a._count;
		if (a._count >= 65536) {
			std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: too much keyframes: " << filepath << std::endl;
			return;
		}
		if (a._key != nullptr) {
			std::cout << "R3DLoader.h(cat::R3DBones::load(const char* filepath)) warning: keyframes with the same name: " << filepath << std::endl;
			delete[] a._key; a._key = nullptr;
		}
		a._key = new glm::mat4[a._count];
		for (unsigned int i = 0; i < a._count; i++) {
			ifs >> a._key[i];
		}
		counter++;
	}
}

void cat::R3DAnimation::load(const char* ptr, size_t size, const R3DBones& bn)
{
}


void cat::R3DAnimation::update(unsigned int time, const R3DBones& bn) const 
{
	auto v = bn.getTransformationMatrix();
	for (unsigned int i = 0; i < _count; i++) {
		auto& a = _anim[i];
		v[a._index] = a.interpolation(time);
	}
}

