#include <iostream>
#include "terrain.h"
#include "R2DLoader.h"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"

using glm::vec3;
using glm::cross;
using glm::normalize;

static void genTerrain(unsigned char* data, float* xyz, float* uv, float* str, unsigned int* idcs, int width, int height, int chn, float Yscale, float XZscale, float UVscale, float XOffset, float Zoffset, float Yoffset)
{
	// width  >= 2
	// height >= 2
	

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			size_t idx2 = (size_t)i * 2 * width + (size_t)j * 2;
			size_t idx3 = (size_t)i * 3 * width + (size_t)j * 3;
			size_t idxx = (size_t)i * chn * width + (size_t)j * 3;
			xyz[idx3 + 0] = XZscale * j + XOffset;				// x
			xyz[idx3 + 1] = Yscale * data[idxx + 0] + Yoffset;	// y
			xyz[idx3 + 2] = XZscale * i + Zoffset;				// z

			uv[idx2 + 0] = UVscale * j;
			uv[idx2 + 1] = UVscale * i;
		}
	}
	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			size_t idx3 = (size_t)i * 3 * width + (size_t)j * 3;

			vec3 p0(xyz[idx3 + 0], xyz[idx3 + 1], xyz[idx3 + 2]);
			vec3 p1(xyz[idx3 + 0 - 3], xyz[idx3 + 1 - 3], xyz[idx3 + 2 - 3]);
			vec3 p2(xyz[idx3 + 0 + 3], xyz[idx3 + 1 + 3], xyz[idx3 + 2 + 3]);
			vec3 p3(xyz[idx3 + 0 - 3 * (size_t)width], xyz[idx3 + 1 - 3 * (size_t)width], xyz[idx3 + 2 - 3 * (size_t)width]);
			vec3 p4(xyz[idx3 + 0 + 3 * (size_t)width], xyz[idx3 + 1 + 3 * (size_t)width], xyz[idx3 + 2 + 3 * (size_t)width]);
			p1 -= p0;
			p2 -= p0;
			p3 -= p0;
			p4 -= p0;
			vec3 v31 = normalize(cross(p3, p1));
			vec3 v42 = normalize(cross(p4, p2));
			vec3 fnl = normalize(v31 + v42);

			str[idx3 + 0] = fnl.x;
			str[idx3 + 1] = fnl.y;
			str[idx3 + 2] = fnl.z;
		}
	}

	// x = 0
	for (int j = 0; j < width; j++) {
		const int i = 0;
		size_t idxthis = ((size_t)i + 0) * 3 * width + ((size_t)j + 0) * 3; // this one
		size_t idxinne = ((size_t)i + 1) * 3 * width + ((size_t)j + 0) * 3; // inner one
		str[idxthis + 0] = str[idxinne + 0];
		str[idxthis + 1] = str[idxinne + 1];
		str[idxthis + 2] = str[idxinne + 2];
	}

	// y = 0
	for (int i = 0; i < height; i++) {
		const int j = 0;
		size_t idxthis = ((size_t)i + 0) * 3 * width + ((size_t)j + 0) * 3; // this one
		size_t idxinne = ((size_t)i + 0) * 3 * width + ((size_t)j + 1) * 3; // inner one
		str[idxthis + 0] = str[idxinne + 0];
		str[idxthis + 1] = str[idxinne + 1];
		str[idxthis + 2] = str[idxinne + 2];
	}

	// x = w-1
	for (int i = 0; i < height; i++) {
		const int j = width - 1;
		size_t idxthis = ((size_t)i + 0) * 3 * width + ((size_t)j + 0) * 3;	 // this one
		size_t idxinne = ((size_t)i + 0) * 3 * width + ((size_t)j - 1) * 3;	 // inner one
		str[idxthis + 0] = str[idxinne + 0];
		str[idxthis + 1] = str[idxinne + 1];
		str[idxthis + 2] = str[idxinne + 2];
	}


	// y = h-1
	for (int j = 0; j < width; j++) {
		const int i = height - 1;
		size_t idxthis = ((size_t)i + 0) * 3 * width + ((size_t)j + 0) * 3;	 // this one
		size_t idxinne = ((size_t)i - 1) * 3 * width + ((size_t)j + 0) * 3;	 // inner one
		str[idxthis + 0] = str[idxinne + 0];
		str[idxthis + 1] = str[idxinne + 1];
		str[idxthis + 2] = str[idxinne + 2];
	}

	size_t counter = 0;
	for (int i = 0; i < height - 1; i++) {
		for (int j = 0; j < width - 1; j++) {
			idcs[counter++] = (j + 0) + (i + 0) * width;
			idcs[counter++] = (j + 0) + (i + 1) * width;
			idcs[counter++] = (j + 1) + (i + 1) * width;
			idcs[counter++] = (j + 0) + (i + 0) * width;
			idcs[counter++] = (j + 1) + (i + 1) * width;
			idcs[counter++] = (j + 1) + (i + 0) * width;
		}
	}
}
cat::terrainBlock::terrainBlock()
{
	_width = _height = 0;
}
void cat::terrainBlock::load(const char* filepath, float Yscale, float XZscale, float UVscale, float XOffset, float Zoffset, float Yoffset)
{
	R2DLoader ld;
	ld.load(filepath, false);
	auto data = ld.getData();
	if (!data) {
		std::cout << "cat::terrainBlock: warning: can't open image file: " << filepath << std::endl;
		return;
	}
	_width = ld.getWidth();
	_height = ld.getHeight();
	if (_width == 1 || _height == 1) {
		std::cout << "cat::terrainBlock: warning: illegal image size: " << filepath << std::endl;
		return;
	}
	int chn = ld.getChannel();
	float* xyz = new float[(size_t)_width * (size_t)_height * 3];
	float*  uv = new float[(size_t)_width * (size_t)_height * 2];
	float* str = new float[(size_t)_width * (size_t)_height * 3];
	unsigned int* idcs = new unsigned int[(size_t)(_width - 1) * (size_t)(_height - 1) * 6];

	::genTerrain(data, xyz, uv, str, idcs, _width, _height, chn, Yscale, XZscale, UVscale, XOffset, Zoffset, Yoffset);
	static_mesh::load(xyz, uv, str, idcs, _width * _height, (_width - 1) * (_height - 1) * 6);
	delete[] xyz;
	delete[] uv;
	delete[] str;
	delete[] idcs;
}

void cat::terrainBlock::load(R2DLoader& ld, float Yscale, float XZscale, float UVscale, float XOffset, float Zoffset, float Yoffset)
{
	auto data = ld.getData();
	if (!data) {
		std::cout << "cat::terrainBlock: warning: can't open image file: unknown file( from R2DLoader )" << std::endl;
		return;
	}
	_width = ld.getWidth();
	_height = ld.getHeight();
	if (_width == 1 || _height == 1) {
		std::cout << "cat::terrainBlock: warning: illegal image size: unknown file( from R2DLoader )" << std::endl;
		return;
	}
	int chn = ld.getChannel();
	float* xyz = new float[(size_t)_width * (size_t)_height * 3];
	float* uv = new float[(size_t)_width * (size_t)_height * 2];
	float* str = new float[(size_t)_width * (size_t)_height * 3];
	unsigned int* idcs = new unsigned int[(size_t)(_width - 1) * (size_t)(_height - 1) * 6];
	::genTerrain(data, xyz, uv, str, idcs, _width, _height, chn, Yscale, XZscale, UVscale, XOffset, Zoffset, Yoffset);
	static_mesh::load(xyz, uv, str, idcs, _width * _height, (_width - 1) * (_height - 1) * 6);
	delete[] xyz;
	delete[] uv;
	delete[] str;
	delete[] idcs;
}