/*
//@author: zark
//@time: 2020 / 5/ 30
//remarks:
// vao	<==> vertex array object
// vbo	<==> vertex buffer object
// vibo	<==> index buffer object
*/
#pragma once

namespace cat
{
	enum class VERTEX_TYPE {
		VT_FLOAT,
		VT_INT,
		VT_DOUBLE,
	};
	class vbo final
	{
		unsigned int _id;
		vbo(const vbo&) { _id = 0; }
	public:
		vbo();
		~vbo();

		void create(const void* data, unsigned int sizeInBytes);
		void create_dynamic(const void* data, unsigned int sizeInBytes);
		void writeData_dynamic(const void* data, unsigned int offset, unsigned int sizeInBytes);
		void bind() const;
		void unbind() const;
		void release();
		inline unsigned int getID() const { return _id; }
	};
	class vibo final
	{
		unsigned int _id;
		unsigned int _count;
		vibo(const vibo&) { _id = _count = 0; }
	public:
		vibo();
		~vibo();

		void create(const unsigned int* data, unsigned int count);

		void bind() const;
		void unbind() const;
		void release();

		inline unsigned int getCount() const { return _count; }
		inline unsigned int getID() const { return _id; }

	};
	class vao final
	{
		unsigned int _id;
		vao(const vao&) { _id = 0; }
	public:
		vao();
		~vao();

		void begin();
		void addBuffer(const vbo& vbo, cat::VERTEX_TYPE type, int count, int layout);
		void end();

		void bind() const;
		void unbind() const;
		void release();
		inline unsigned int getID() const { return _id; }

	};
}