/*
@author:	zrk
@date:		2020 / 6 / 1
*/
#pragma once

namespace cat
{
	class R2DLoader final
	{
		unsigned char* _data;
		int _width;
		int _height;
		int _channel;
		R2DLoader(const R2DLoader&) { _data = nullptr; _width = _height = _channel = 0; }
	public:
		R2DLoader();
		~R2DLoader();
		void load(const char* filepath, bool flip);
		void load(const char* data, unsigned int size, bool flip);
		void free();

		inline unsigned char* getData() const { return _data; }
		inline int getWidth()   const { return _width; }
		inline int getHeight()  const { return _height; }
		inline int getChannel() const { return _channel; }
	};
}