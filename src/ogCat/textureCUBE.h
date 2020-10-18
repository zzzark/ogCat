/*
@author:	zrk
@date:		2020/6/13
*/
#pragma once
#include "texture2D.h"
namespace cat
{
	class textureCUBE final
	{
		unsigned int _id;
		textureCUBE(const textureCUBE&) { _id = 0; };
	public:
		textureCUBE();
		~textureCUBE();

		void create(const char* f0, const char* f1, const char* f2, const char* f3, const char* f4, const char* f5);
		void begin();
		void createFace(unsigned int index, unsigned int width, unsigned int height, TEXTURE_TYPE type, TEXTURE_DATA_TYPE dtype, const void* data);
		void createFaceEX(unsigned int index, unsigned int width, unsigned int height, unsigned int channel, unsigned int bit, TEXTURE_TYPE type, TEXTURE_DATA_TYPE dtype, const void* data);
		void end();

		void active(unsigned int slot = 0) const;
		void bind() const;
		void unbind() const;
		void release();

		void setfilter(TEXTURE_FILTER filter) const;
		void setwrap(TEXTURE_WRAP wrap, float r = 0, float g = 0, float b = 0, float a = 0) const;

		inline unsigned int getID() const { return _id; }
	};
}