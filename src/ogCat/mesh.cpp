#include "mesh.h"
#include "dbg.h"

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
void cat::static_mesh::load(const char* filepath)
{
	R3DLoader::load(filepath);
	if (this->getVtsCnt() <= 0) {
		std::cout << "cat::static_mesh: warning: can't load R3D file: " << filepath << std::endl;
		return;
	}
	mesh::_xyz.create(R3DLoader::getXYZ(), this->getVtsCnt() * 3 * sizeof(float));
	mesh:: _uv.create(R3DLoader::getUV(),  this->getVtsCnt() * 2 * sizeof(float));
	mesh::_str.create(R3DLoader::getSTR(), this->getVtsCnt() * 3 * sizeof(float));
	mesh:: _ib.create(R3DLoader::getIds(), this->getIdsCnt());

	_vao.begin();
	_vao.addBuffer(mesh::_xyz, VERTEX_TYPE::VT_FLOAT, 3, 0);
	_vao.addBuffer(mesh:: _uv, VERTEX_TYPE::VT_FLOAT, 2, 1);
	_vao.addBuffer(mesh::_str, VERTEX_TYPE::VT_FLOAT, 3, 2);
	_vao.end();
	free();	// free the data after loading them into GPU memory
}

void cat::static_mesh::load(const char* ptr, size_t size)
{
	R3DLoader::load(ptr, size);
	CAT_ASSERT(this->getVtsCnt() > 0);
	mesh::_xyz.create(R3DLoader::getXYZ(), this->getVtsCnt() * 3 * sizeof(float));
	mesh:: _uv.create(R3DLoader::getUV(),  this->getVtsCnt() * 2 * sizeof(float));
	mesh::_str.create(R3DLoader::getSTR(), this->getVtsCnt() * 3 * sizeof(float));
	mesh::_ib.create(R3DLoader::getIds(), this->getIdsCnt());
	_vao.begin();
	_vao.addBuffer(mesh::_xyz, VERTEX_TYPE::VT_FLOAT, 3, 0);
	_vao.addBuffer(mesh::_uv, VERTEX_TYPE::VT_FLOAT, 2, 1);
	_vao.addBuffer(mesh::_str, VERTEX_TYPE::VT_FLOAT, 3, 2);
	_vao.end();
	free();	// free the data after loading them into GPU memory
}

//void cat::static_mesh::draw() const
//{
//	if (_ib.getCount() == 0)
//		return;
//	_vao.bind();
//	_ib.bind();
//	glDrawElements(GL_TRIANGLES, _ib.getCount(), GL_UNSIGNED_INT, nullptr);
//}

void cat::static_mesh::load(const float* position, const float* uvCoords, const float* normal, const unsigned int* indices, unsigned int vtsCount, unsigned int idsCount)
{
	int layout = 0;
	_vao.begin();
	if (position) {
		mesh::_xyz.create(position, vtsCount * 3 * sizeof(float));
		mesh::_vao.addBuffer(mesh::_xyz, VERTEX_TYPE::VT_FLOAT, 3, layout++);
	}
	else {
		CAT_ASSERT(false);	// position info is necessary
	}
	if (uvCoords) {
		mesh:: _uv.create(uvCoords, vtsCount * 2 * sizeof(float));
		mesh::_vao.addBuffer(mesh:: _uv, VERTEX_TYPE::VT_FLOAT, 2, layout++);
	}
	if (normal) {
		mesh::_str.create(normal, vtsCount * 3 * sizeof(float));
		mesh::_vao.addBuffer(mesh::_str, VERTEX_TYPE::VT_FLOAT, 3, layout++);
	}
	_vao.end();
	_ib.create(indices, idsCount);
}
