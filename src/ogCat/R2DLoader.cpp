#include "R2DLoader.h"
#include "stb_image/stb_image.h"
cat::R2DLoader::R2DLoader()
{
	_data = nullptr;
	_width = _height = _channel = 0;
}

cat::R2DLoader::~R2DLoader()
{
	free();
}

void cat::R2DLoader::load(const char* filepath, bool flip)
{
	stbi_set_flip_vertically_on_load(flip);
	_data = stbi_load(filepath, &_width, &_height, &_channel, 0);
}

void cat::R2DLoader::load(const char* data, unsigned int size, bool flip)
{
	stbi_set_flip_vertically_on_load(flip);
	_data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(data), size, &_width, &_height, &_channel, 0);
}

void cat::R2DLoader::free()
{
	delete[] _data; _data = nullptr;
	_width = _height = _channel = 0;
}
