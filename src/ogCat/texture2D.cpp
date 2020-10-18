#include "dbg.h"
#include "texture2D.h"
#include "R2DLoader.h"
cat::texture2D::texture2D()
{
	_id = 0;
	_width = 0;
	_height = 0;
}

cat::texture2D::~texture2D()
{
	release();
}

void cat::texture2D::setfilter(TEXTURE_FILTER filter) const
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
	CAT_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param));
	CAT_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param));
}
void cat::texture2D::setfilter(TEXTURE_FILTER texFilter, TEXTURE_FILTER mipmapFilter) const
{
	bind();
	GLint param;
	param = 0;
	if (texFilter == TEXTURE_FILTER::TF_NEAREST && mipmapFilter == TEXTURE_FILTER::TF_NEAREST)
		param = GL_NEAREST_MIPMAP_NEAREST;
	if (texFilter == TEXTURE_FILTER::TF_LINEAR  && mipmapFilter == TEXTURE_FILTER::TF_NEAREST)
		param = GL_LINEAR_MIPMAP_NEAREST;
	if (texFilter == TEXTURE_FILTER::TF_NEAREST && mipmapFilter == TEXTURE_FILTER::TF_LINEAR)
		param = GL_NEAREST_MIPMAP_LINEAR;
	if (texFilter == TEXTURE_FILTER::TF_LINEAR  && mipmapFilter == TEXTURE_FILTER::TF_LINEAR)
		param = GL_LINEAR_MIPMAP_LINEAR;
	CAT_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param));

	param = 0;
	switch (texFilter) {
	case TEXTURE_FILTER::TF_LINEAR:
		param = GL_LINEAR;
		break;
	case TEXTURE_FILTER::TF_NEAREST:
		param = GL_NEAREST;
		break;
	}	CAT_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param));
}
void cat::texture2D::setwrap(TEXTURE_WRAP wrap, float r, float g, float b, float a) const
{
	/*
	GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT, or GL_MIRROR_CLAMP_TO_EDGE. GL_CLAMP_TO_EDGE
	*/
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
	if (param == GL_CLAMP_TO_BORDER) {
		GLfloat borderColor[] = { r, g, b, a };
		CAT_CALL(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
	}
}

void cat::texture2D::create(TEXTURE_TYPE type, unsigned int width, unsigned int height, TEXTURE_DATA_TYPE dtype, const void* pixels, bool genMipmap)
{
	release();
	_width = width;
	_height = height;
	
	CAT_CALL(glGenTextures(1, &_id));
	CAT_CALL(glBindTexture(GL_TEXTURE_2D, _id));
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
	CAT_CALL(glTexImage2D(GL_TEXTURE_2D, 0, glinternalformat, _width, _height, 0, glformat, gltype, pixels));
	if (genMipmap) {
		CAT_CALL(glGenerateMipmap(GL_TEXTURE_2D));
	}
}

void cat::texture2D::createEX(TEXTURE_TYPE type, unsigned int bit, unsigned int width, unsigned int height, TEXTURE_DATA_TYPE dtype, const void* pixels, bool genMipmap)
{
	release();
	_width = width;
	_height = height;

	CAT_CALL(glGenTextures(1, &_id));
	CAT_CALL(glBindTexture(GL_TEXTURE_2D, _id));
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
	CAT_CALL(glTexImage2D(GL_TEXTURE_2D, 0, glinternalformat, _width, _height, 0, glformat, gltype, pixels));
	if (genMipmap) {
		CAT_CALL(glGenerateMipmap(GL_TEXTURE_2D));
	}
}

void cat::texture2D::createFromFile(const char* filepath, bool genMipmap)
{
	int channel;
	int width;
	int height;
	R2DLoader loader;
	loader.load(filepath, true);
	void* pixels = loader.getData();
	width = loader.getWidth();
	height = loader.getHeight();
	channel = loader.getChannel();
	if (!pixels) {
		std::cout << "warning: can't load image: " << filepath << std::endl;
		return;
	}
	CAT_ASSERT(width > 0 && height > 0);
	if (channel == 3) {
		this->create(TEXTURE_TYPE::TT_RGB, width, height, TEXTURE_DATA_TYPE::TDT_BYTE, pixels, genMipmap);
	}
	else {
		this->create(TEXTURE_TYPE::TT_RGBA, width, height, TEXTURE_DATA_TYPE::TDT_BYTE, pixels, genMipmap);
	}
}
void cat::texture2D::active(unsigned int slot) const
{
	//CAT_CALL(glActiveTexture(GL_TEXTURE0 + slot));
	//CAT_CALL(glEnable(GL_TEXTURE_2D));
	//CAT_CALL(glBindTexture(GL_TEXTURE_2D, _id));
	glBindTextureUnit(slot, _id);
}
void cat::texture2D::bind() const
{
	glBindTexture(GL_TEXTURE_2D, _id);
}

void cat::texture2D::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void cat::texture2D::release()
{
	if (_id) {
		CAT_CALL(glDeleteTextures(1, &_id));
	}
}
