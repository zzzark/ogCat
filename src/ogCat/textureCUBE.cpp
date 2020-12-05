#include "dbg.h"
#include "R2DLoader.h"
#include "texture2D.h"
#include "textureCUBE.h"

cat::textureCUBE::textureCUBE()
{
	_id = 0;
}

cat::textureCUBE::~textureCUBE()
{
	release();
}

void cat::textureCUBE::create(const char* f0, const char* f1, const char* f2, const char* f3, const char* f4, const char* f5)
{
	const char* files[6] = { f0,f1,f2,f3,f4,f5 };
	this->begin();
	for (int i = 0; i < 6; i++) {
		int ww, hh, nrChannels;
		R2DLoader loader;
		loader.load(files[i], false);
		unsigned char* data = loader.getData();
		ww = loader.getWidth();
		hh = loader.getHeight();
		nrChannels = loader.getChannel();

		if (data == nullptr) {
			std::cout << "cat::textureCUBE: warning: can't load image file: " << files[i] << std::endl;
		}
		if (ww != hh) {
			std::cout << "error: width != height, and both are set to 100 by default, file: " << files[i] << std::endl;
			data = nullptr;
			ww = hh = 100;
		}
		if (ww % 4 != 0) {
			std::cout << "warning: width % 4 != 0, file: " << files[i] << std::endl;
		}
		this->createFace(i, ww, hh, (nrChannels == 3) ? TEXTURE_TYPE::TT_RGB : TEXTURE_TYPE::TT_RGBA, TEXTURE_DATA_TYPE::TDT_BYTE, data);
	}
	this->end();
}

void cat::textureCUBE::begin()
{
	release();
	CAT_CALL(glGenTextures(1, &_id));
	CAT_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, _id));
}

void cat::textureCUBE::createFace(unsigned int index, unsigned int width, unsigned int height, TEXTURE_TYPE type, TEXTURE_DATA_TYPE dtype, const void* data)
{
	GLint  glinternalformat = 0;	// using base internal format
	GLenum glformat = 0;
	GLenum gltype = 0;
	switch (type) {
	case TEXTURE_TYPE::TT_R:
		glinternalformat = GL_RED;
		glformat = GL_RGB;
		break;
	case TEXTURE_TYPE::TT_RG:
		glinternalformat = GL_RG;
		glformat = GL_RG;
		break;
	case TEXTURE_TYPE::TT_RGB:
		glinternalformat = GL_RGB;
		glformat = GL_RGB;
		break;
	case TEXTURE_TYPE::TT_RGBA:
		glinternalformat = GL_RGBA;
		glformat = GL_RGBA;
		break;
	case TEXTURE_TYPE::TT_DEPTH:
		glinternalformat = GL_DEPTH_COMPONENT;
		glformat = GL_DEPTH_COMPONENT;
		break;
	case TEXTURE_TYPE::TT_DEPTH_STENCIL:
		glinternalformat = GL_DEPTH_STENCIL;
		glformat = GL_DEPTH_STENCIL;
		break;
	default:
		CAT_ASSERT(0);
	};
	switch (dtype) {
	case TEXTURE_DATA_TYPE::TDT_BYTE:
		gltype = GL_UNSIGNED_BYTE;
		break;
	case TEXTURE_DATA_TYPE::TDT_FLOAT:
		gltype = GL_FLOAT;
	}
	CAT_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, glinternalformat, width, height, 0, glformat, gltype, data));
}

void cat::textureCUBE::createFaceEX(unsigned int index, unsigned int width, unsigned int height, unsigned int channel, unsigned int bit, TEXTURE_TYPE type, TEXTURE_DATA_TYPE dtype, const void* data)
{
	GLint  glinternalformat = 0;
	GLenum glformat = 0;
	GLenum gltype = 0;
	switch (type) {
	case TEXTURE_TYPE::TT_R:
		glinternalformat = (bit == 32 ? GL_R32F : GL_R16F);
		glformat = GL_RED;
		break;
	case TEXTURE_TYPE::TT_RG:
		glinternalformat = (bit == 32 ? GL_RG32F : GL_RG16F);
		glformat = GL_RG;
		break;
	case TEXTURE_TYPE::TT_RGB:
		glinternalformat = (bit == 32 ? GL_RGB32F : GL_RGB16F);
		glformat = GL_RGB;
		break;
	case TEXTURE_TYPE::TT_RGBA:
		glinternalformat = (bit == 32 ? GL_RGBA32F : GL_RGBA16F);
		glformat = GL_RGBA;
		break;
	case TEXTURE_TYPE::TT_DEPTH:
		glinternalformat = GL_DEPTH_COMPONENT;
		glformat = GL_DEPTH_COMPONENT;
		break;
	case TEXTURE_TYPE::TT_DEPTH_STENCIL:
		glinternalformat = GL_DEPTH_STENCIL;
		glformat = GL_DEPTH_STENCIL;
		break;
	default:
		CAT_ASSERT(0);
	};
	switch (dtype) {
	case TEXTURE_DATA_TYPE::TDT_BYTE:
		gltype = GL_UNSIGNED_BYTE;
		break;
	case TEXTURE_DATA_TYPE::TDT_FLOAT:
		gltype = GL_FLOAT;
	}
	CAT_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, glinternalformat, width, height, 0, glformat, gltype, data));
}

void cat::textureCUBE::end()
{
	CAT_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	CAT_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	CAT_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
	CAT_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CAT_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	CAT_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void cat::textureCUBE::active(unsigned int slot) const
{
	glBindTextureUnit(slot, _id);
}

void cat::textureCUBE::bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
}

void cat::textureCUBE::unbind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void cat::textureCUBE::release()
{
	if (_id) {
		CAT_CALL(glDeleteTextures(1, &_id));
		_id = 0;
	}
}

void cat::textureCUBE::setfilter(TEXTURE_FILTER filter) const
{
	bind();
	GLint param = 0;
	switch (filter) {
	case TEXTURE_FILTER::TF_LINEAR:
		param = GL_LINEAR;
		break;
	case TEXTURE_FILTER::TF_NEAREST:
		param = GL_NEAREST;
		break;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, param);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, param);
	unbind();
}

void cat::textureCUBE::setwrap(TEXTURE_WRAP wrap, float r, float g, float b, float a) const
{
	bind();
	GLint param = 0;
	switch (wrap) {
	case TEXTURE_WRAP::TW_REPEAT:
		param = GL_REPEAT;
		break;
	case TEXTURE_WRAP::TW_MIRRORED:
		param = GL_MIRRORED_REPEAT;
		break;
	case TEXTURE_WRAP::TW_CLAMP_TO_EDGE:
		param = GL_CLAMP_TO_EDGE;
		break;
	case TEXTURE_WRAP::TW_CLAMP_TO_BORDER:
		param = GL_CLAMP_TO_BORDER;
		break;
	}
	CAT_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param));
	CAT_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param));
	CAT_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, param));
	if (param == GL_CLAMP_TO_BORDER) {
		GLfloat borderColor[] = { r, g, b, a };
		CAT_CALL(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
	}
	unbind();
}
