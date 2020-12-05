#include "particle.h"
#include "dbg.h"
static const char* spk_vs =
"#version 450								\n"
"layout (location = 0) in vec3  in_pos;		\n"
"layout (location = 1) in float in_int;	    \n"
"out float out_int;                         \n"
"void main()								\n"
"{											\n"
"	gl_Position = vec4(in_pos, 1.0f);		\n"
"   out_int = in_int;                       \n"
"}											\n"
;
static const char* spk_gs =
"#version 450								\n"
"layout (points) in;						\n"
"layout (triangle_strip) out;				\n"
"layout (max_vertices = 4) out;				\n"
"uniform vec3 u_right;                      \n"
"uniform vec3 u_up;                         \n"
"uniform float u_size = 1.0f;               \n"
"in float out_int[];                        \n"
"out float v_int;                           \n"
"											\n"
"out vec2 v_uv;	        					\n"
"uniform mat4 u_MVP;						\n"
"void main()								\n"
"{											\n"
"   v_int = out_int[0];                     \n"
"	vec3 pos = gl_in[0].gl_Position.xyz;	\n"
"											\n"
"	pos -= (u_right * 0.5f * u_size);		\n"
"	gl_Position = u_MVP * vec4(pos, 1.0f);	\n"
"	v_uv = vec2(0, 0);				    	\n"
"	EmitVertex();							\n"
"											\n"
"	pos += u_up * u_size;				    \n"
"	gl_Position = u_MVP * vec4(pos, 1.0f);	\n"
"	v_uv = vec2(0, 1);						\n"
"	EmitVertex();							\n"
"											\n"
"	pos -= u_up * u_size;					\n"
"	pos += u_right * u_size;				\n"
"	gl_Position = u_MVP * vec4(pos, 1.0f);	\n"
"	v_uv = vec2(1, 0);						\n"
"	EmitVertex();							\n"
"											\n"
"	pos += u_up * u_size;					\n"
"	gl_Position = u_MVP * vec4(pos, 1.0f);	\n"
"	v_uv = vec2(1, 1);						\n"
"	EmitVertex();							\n"
"											\n"
"	EndPrimitive();							\n"
"}											\n"
;
static const char* spk_fs =
"#version 450								       \n"
"uniform float u_int = 1.0f;                       \n"
"uniform sampler2D u_tex;					       \n"
"flat in float v_int;                              \n"
"in  vec2 v_uv;								       \n"
"out vec3 color;							       \n"
"void main()								       \n"
"{											       \n"
"    color = texture2D(u_tex, v_uv).xyz            \n"
"            * v_int * u_int;                      \n"
"}											       \n"
;
void cat::sparks::setSize(float sz) const
{
	shader::uniform u_size = _shd["u_size"];
	u_size = sz;
}
void cat::sparks::setIntensity(float v) const
{
	shader::uniform u_int = _shd["u_int"];
	u_int = v;
}
void cat::sparks::begin(const char* filepath, unsigned int count)
{
	_shd.begin();
	_shd.load(spk_vs, shader::SHADER_TYPE::VERTEX_SHADER);
	_shd.load(spk_gs, shader::SHADER_TYPE::GEOMETRY_SHADER);
	_shd.load(spk_fs, shader::SHADER_TYPE::FRAGMENT_SHADER);
	_shd.end();
	shader::uniform u_tex = _shd["u_tex"];  u_tex = (int)0;

	_loc_mvp = _shd.getlocation("u_MVP");
	_loc_right = _shd.getlocation("u_right");
	_loc_up    = _shd.getlocation("u_up");
	_tex.createFromFile(filepath, true);
	_tex.setfilter(cat::TEXTURE_FILTER::TF_LINEAR, cat::TEXTURE_FILTER::TF_LINEAR);
	_count = count;
	_vao.begin();
}

void cat::sparks::addPos(const void* data, unsigned int sizeInBytes)
{
	_pos.create(data, sizeInBytes);
	_vao.addBuffer(_pos, cat::VERTEX_TYPE::VT_FLOAT, 3, 0);
}

void cat::sparks::addInt(const void* data, unsigned int sizeInBytes)
{
	_int.create(data, sizeInBytes);
	_vao.addBuffer(_int, cat::VERTEX_TYPE::VT_FLOAT, 1, 1);
}


void cat::sparks::end()
{
	_vao.end();
}

void cat::sparks::render(const gbuffer& gbuf, const camera& cam)
{
	/*
	  + prepare gbuffer
	*/
	gbuf.bind();
	gbuf.switchBuffers(cat::gbuffer::BUFFER_LAYER::EMMISIVE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	_tex.active(0);
	_shd.bind();
	_vao.bind();
	_shd.setmat4(_loc_mvp, cam.comb());
	glm::vec3 right = glm::normalize(glm::cross(cam.getEye() - cam.getAt(), glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::normalize(glm::cross(right, cam.getEye() - cam.getAt()));
	_shd.setvec3(_loc_right, right);
	_shd.setvec3(_loc_up, up);
	glDrawArrays(GL_POINTS, 0, _count);
	glDisable(GL_BLEND);
}

cat::sparks::~sparks()
{
}

//=========================================================================================//

void cat::weed::create(const char* filepath, unsigned int count, float scale, const float* data)
{
	_tex.createFromFile(filepath, true);
	_tex.setwrap(cat::TEXTURE_WRAP::TW_CLAMP_TO_EDGE);
	float* xyz = new float[(size_t)count * 3 * 4 * 2];
	float* uv  = new float[(size_t)count * 2 * 4 * 2];
	float* str = new float[(size_t)count * 3 * 4 * 2];
	unsigned int* idx = new unsigned int[(size_t)count * 6 * 2];
	for (unsigned int i = 0; i < count; i++) {
		glm::vec3& v0 = *(glm::vec3*)(&xyz[i * 12 + 0]);
		glm::vec3& v1 = *(glm::vec3*)(&xyz[i * 12 + 3]);
		glm::vec3& v2 = *(glm::vec3*)(&xyz[i * 12 + 6]);
		glm::vec3& v3 = *(glm::vec3*)(&xyz[i * 12 + 9]);
		glm::vec3& p = *(glm::vec3*)(&data[i * 3]);
		v0.x = p.x - scale * 0.5f;
		v0.y = p.y;
		v0.z = p.z;
		v1.x = p.x - scale * 0.5f;
		v1.y = p.y + scale;
		v1.z = p.z;
		v2.x = p.x + scale * 0.5f;
		v2.y = p.y;
		v2.z = p.z;
		v3.x = p.x + scale * 0.5f;
		v3.y = p.y + scale;
		v3.z = p.z;
		uv[i * 8 + 0] = 0.0f;
		uv[i * 8 + 1] = 0.0f;
		uv[i * 8 + 2] = 0.0f;
		uv[i * 8 + 3] = 1.0f;
		uv[i * 8 + 4] = 1.0f;
		uv[i * 8 + 5] = 0.0f;
		uv[i * 8 + 6] = 1.0f;
		uv[i * 8 + 7] = 1.0f;

		str[i * 12 +  0] = 0.0f; // X
		str[i * 12 +  1] = 1.0f; // Y
		str[i * 12 +  2] = 0.0f; // Z
		str[i * 12 +  3] = 0.0f; // X
		str[i * 12 +  4] = 1.0f; // Y
		str[i * 12 +  5] = 0.0f; // Z
		str[i * 12 +  6] = 0.0f; // X
		str[i * 12 +  7] = 1.0f; // Y
		str[i * 12 +  8] = 0.0f; // Z
		str[i * 12 +  9] = 0.0f; // X
		str[i * 12 + 10] = 1.0f; // Y
		str[i * 12 + 11] = 0.0f; // Z

		idx[i * 6 + 0] = 0 + i * 4;
		idx[i * 6 + 1] = 1 + i * 4;
		idx[i * 6 + 2] = 2 + i * 4;
		idx[i * 6 + 3] = 1 + i * 4;
		idx[i * 6 + 4] = 2 + i * 4;
		idx[i * 6 + 5] = 3 + i * 4;
	}
	for (unsigned int i = count; i < count * 2; i++) {
		glm::vec3& v0 = *(glm::vec3*)(&xyz[i * 12 + 0]);
		glm::vec3& v1 = *(glm::vec3*)(&xyz[i * 12 + 3]);
		glm::vec3& v2 = *(glm::vec3*)(&xyz[i * 12 + 6]);
		glm::vec3& v3 = *(glm::vec3*)(&xyz[i * 12 + 9]);
		glm::vec3& p = *(glm::vec3*)(&data[(i - count) * 3]);
		v0.z = p.z - scale * 0.5f;
		v0.y = p.y;
		v0.x = p.x;
		v1.z = p.z - scale * 0.5f;
		v1.y = p.y + scale;
		v1.x = p.x;
		v2.z = p.z + scale * 0.5f;
		v2.y = p.y;
		v2.x = p.x;
		v3.z = p.z + scale * 0.5f;
		v3.y = p.y + scale;
		v3.x = p.x;
		uv[i * 8 + 0] = 0.0f;
		uv[i * 8 + 1] = 0.0f;
		uv[i * 8 + 2] = 0.0f;
		uv[i * 8 + 3] = 1.0f;
		uv[i * 8 + 4] = 1.0f;
		uv[i * 8 + 5] = 0.0f;
		uv[i * 8 + 6] = 1.0f;
		uv[i * 8 + 7] = 1.0f;

		str[i * 12 + 0] = 0.0f; // X
		str[i * 12 + 1] = 1.0f; // Y
		str[i * 12 + 2] = 0.0f; // Z
		str[i * 12 + 3] = 0.0f; // X
		str[i * 12 + 4] = 1.0f; // Y
		str[i * 12 + 5] = 0.0f; // Z
		str[i * 12 + 6] = 0.0f; // X
		str[i * 12 + 7] = 1.0f; // Y
		str[i * 12 + 8] = 0.0f; // Z
		str[i * 12 + 9] = 0.0f; // X
		str[i * 12 + 10] = 1.0f; // Y
		str[i * 12 + 11] = 0.0f; // Z

		idx[i * 6 + 0] = 0 + i * 4;
		idx[i * 6 + 1] = 1 + i * 4;
		idx[i * 6 + 2] = 2 + i * 4;
		idx[i * 6 + 3] = 2 + i * 4;
		idx[i * 6 + 4] = 1 + i * 4;
		idx[i * 6 + 5] = 3 + i * 4;
	}

	_xyz.create(xyz, (size_t)count * 3 * 4 * 2 * sizeof(float));
	 _uv.create( uv, (size_t)count * 2 * 4 * 2 * sizeof(float));
	_str.create(str, (size_t)count * 3 * 4 * 2 * sizeof(float));
	_ibo.create(idx, (size_t)count * 6 * 2);
	_vao.begin();
	_vao.addBuffer(_xyz, cat::VERTEX_TYPE::VT_FLOAT, 3, 0);
	_vao.addBuffer( _uv, cat::VERTEX_TYPE::VT_FLOAT, 2, 1);
	_vao.addBuffer(_str, cat::VERTEX_TYPE::VT_FLOAT, 3, 2);
	_vao.end();
	delete[] xyz;
	delete[] uv;
	delete[] str;
	delete[] idx;
}

void cat::weed::draw(const camera& cam)
{
	if (_ibo.getCount() == 0)
		return;
	glDisable(GL_CULL_FACE);
	_tex.active();
	_vao.bind();
	_ibo.bind();
	glDrawElements(GL_TRIANGLES, _ibo.getCount(), GL_UNSIGNED_INT, nullptr);
	glEnable(GL_CULL_FACE);
}

cat::weed::~weed()
{
}

void cat::sparksController::begin(const char* vs, const char* var0, const char* var1)
{
	const char* vars[2] = {
		var0,
		var1,
	};
	_shd.begin();
	_shd.load(vs, shader::SHADER_TYPE::VERTEX_SHADER);
	_shd.end(vars, 2);

	for (auto& e : _vbos) {
		delete e._ptr;
	}
	_vbos.clear();
}

// =========================================================================== //

void cat::sparksController::addAttribute(const void* data, unsigned int sizeInBytes, int count, cat::VERTEX_TYPE type)
{
	auto ptr = new cat::vbo;
	ptr->create(data, sizeInBytes);
	_vbos.push_back({ ptr, count, type });
}

void cat::sparksController::end()
{
	unsigned int layout = 0;
	_vao.begin();
	for (auto& e : _vbos) {
		_vao.addBuffer(*e._ptr, e._type, e._count, layout++);
	}
	_vao.end();
}

void cat::sparksController::update(const sparks& spk)
{
	_shd.bind();
	feedback::bind(spk.getPos(), 0);
	feedback::bind(spk.getInt(), 1);
	feedback::begin(cat::feedback::FB_TYPE::FB_POINTS);
	_vao.bind();
	glDrawArrays(GL_POINTS, 0, spk.getCount());
	feedback::end();
	feedback::unbind(spk.getInt());
	feedback::unbind(spk.getPos());
}

cat::sparksController::~sparksController()
{
	for (auto& e : _vbos) {
		delete e._ptr;
	}
	_vbos.clear();
}
