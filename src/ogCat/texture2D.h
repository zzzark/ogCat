/*
@author:	zrk
@date:		2020/6/2
*/
#pragma once

namespace cat
{
	enum class TEXTURE_TYPE {
		TT_R,
		TT_RG,
		TT_RGB,
		TT_RGBA,
		TT_DEPTH,
		TT_DEPTH_STENCIL,
	};
	enum class TEXTURE_DATA_TYPE {
		TDT_BYTE,
		TDT_FLOAT,
	};
	enum class TEXTURE_FILTER {
		TF_NEAREST,
		TF_LINEAR,
		
	};
	enum class TEXTURE_WRAP {
		TW_REPEAT,
		TW_MIRRORED,
		TW_CLAMP_TO_EDGE,
		TW_CLAMP_TO_BORDER,
	};
	class texture2D final
	{
		unsigned int _id;
		unsigned int _width;
		unsigned int _height;
		texture2D(const texture2D&) { _id = _width = _height = 0; }
	public:
		texture2D();
		~texture2D();

		void setfilter(TEXTURE_FILTER filter) const;
		void setfilter(TEXTURE_FILTER texFilter, TEXTURE_FILTER mipmapFilter) const;
		void setwrap(TEXTURE_WRAP wrap, float r = 0, float g = 0, float b = 0, float a = 0) const;
		void create(TEXTURE_TYPE type, unsigned int width, unsigned int height, TEXTURE_DATA_TYPE dtype, const void* pixels, bool genMipmap);
		void createEX(TEXTURE_TYPE type, unsigned int bit, unsigned int width, unsigned int height, TEXTURE_DATA_TYPE dtype, const void* pixels, bool genMipmap);
		void createFromFile(const char* filepath, bool genMipmap);
		void bind() const;
		void unbind() const;
		void active(unsigned int slot = 0) const;
		void release();


		inline unsigned int getID()     const { return _id; }	// note: for framebuffer
		inline unsigned int getWidth()  const { return _width; }
		inline unsigned int getHeight() const { return _height; }
	};
};