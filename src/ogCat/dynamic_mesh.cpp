#include "dynamic_mesh.h"
#include "dbg.h"

cat::mesh::~mesh() = default;

//=====================================================================================================================//
void cat::dynamic_mesh::load(const char* binFile, R3DBones& bones)
{
	R3DLoader::load(binFile);
	if (this->getVtsCnt() <= 0) {
		std::cout << "cat::dynamic_mesh: warning: can't load R3D file: " << binFile << std::endl;
		return;
	}
	if (this->getW() == nullptr || this->getB() == nullptr) {
		std::cout << "cat::dynamic_mesh: warning: this is not a dynamic mesh file, use static_mesh instead: " << binFile << std::endl;
		free();
		return;
	}
	_vtsCount = this->getVtsCnt();

	_bones = &bones;
	_lastStateDelta = new glm::mat4[_bones->getCount()];
	_currentState = new glm::mat4[_bones->getCount()];

	mesh::_xyz.create(this->getXYZ(), this->getVtsCnt() * 3 * sizeof(float));
	mesh::_uv.create(this->getUV(), this->getVtsCnt() * 2 * sizeof(float));
	mesh::_str.create(this->getSTR(), this->getVtsCnt() * 3 * sizeof(float));
	mesh::_ib.create(this->getIds(), this->getIdsCnt());

	_vao.begin();
	_vao.addBuffer(mesh::_xyz, VERTEX_TYPE::VT_FLOAT, 3, 0);
	_vao.addBuffer(mesh::_uv, VERTEX_TYPE::VT_FLOAT, 2, 1);
	_vao.addBuffer(mesh::_str, VERTEX_TYPE::VT_FLOAT, 3, 2);
	_vao.end();


	_secxyz.create(this->getXYZ(), this->getVtsCnt() * 3 * sizeof(float));
	_secstr.create(this->getSTR(), this->getVtsCnt() * 3 * sizeof(float));
	_w.create(this->getW(), this->getVtsCnt() * 4 * sizeof(float));
	_b.create(this->getB(), this->getVtsCnt() * 4 * sizeof(unsigned int));

	_secvao.begin();
	_vao.addBuffer(_secxyz, VERTEX_TYPE::VT_FLOAT, 3, 0);
	_vao.addBuffer(_secstr, VERTEX_TYPE::VT_FLOAT, 3, 1);
	_vao.addBuffer(_w, VERTEX_TYPE::VT_FLOAT, 4, 2);
	_vao.addBuffer(_b, VERTEX_TYPE::VT_INT, 4, 3);
	_secvao.end();

	free();	// free the data after loading them into GPU memory
}

void cat::dynamic_mesh::update(R3DAnimation& ani, unsigned int tm, const glm::mat4& m)
{
	if (_state == 0) {		// loop
		_accuTime += tm - _lastTime;
		ani.update(_accuTime, *_bones);
		_bones->update(m);
		for (unsigned int i = 0; i < _bones->getCount(); i++) {
			_currentState[i] = _bones->getCombMatrix()[i];
		}
	}
	else if (_state == 1) {	// pause
		ani.update(_accuTime, *_bones);
		_bones->update(m);
		for (unsigned int i = 0; i < _bones->getCount(); i++) {
			_currentState[i] = _bones->getCombMatrix()[i];
		}
	}
	else if (_state == 2) {	// stop
		if (_fadeFactor < 1.0f) {
			_fadeFactor += 0.1f;
			for (unsigned int i = 0; i < _bones->getCount(); i++) {
				_currentState[i] += _lastStateDelta[i] * 0.1f;
			}
		}
	}
	_lastMat = m;
	_lastTime = tm;
}

cat::dynamic_mesh::~dynamic_mesh()
{
	delete[] _lastStateDelta;
	_lastStateDelta = nullptr;
	delete[] _currentState;
	_currentState = nullptr;
}
void cat::dynamic_mesh::loop()
{
	if (_state == 0) return;
	_state = 0;
}
void cat::dynamic_mesh::pause()
{
	if (_state == 1) return;
	_state = 1;
}
void cat::dynamic_mesh::stop(R3DAnimation& ani)
{
	//						//
	// current action state	//
	//						//
	//			 ||			//
	//			 ||			//
	//			 \/			//
	//						//
	// default action state	//
	//						//
	if (_state == 2) return;
	_state = 2;
	ani.update(0, *_bones);
	_bones->update(_lastMat);
	for (unsigned int i = 0; i < _bones->getCount(); i++) {
		_lastStateDelta[i] = _bones->getCombMatrix()[i] - _currentState[i]; // desired - current
	}
	_accuTime = 0;
	_fadeFactor = 0.0f;
}
//=====================================================================================================================//
static const char* vs =
"#version 450 core                                                                         \n"
"layout(location = 0) in vec3  v_pos;                                                      \n"
"layout(location = 1) in vec3  v_nml;                                                      \n"
"layout(location = 2) in vec4  v_w;                                                        \n"
"layout(location = 3) in ivec4 v_b;                                                        \n"
"                                                                                          \n"
"uniform mat4 u_bones[64];                                                                 \n"
"                                                                                          \n"
"layout(location = 0) out vec3 v_xyz;                                                      \n"
"layout(location = 1) out vec3 v_str;                                                      \n"
"void main()                                                                               \n"
"{                                                                                         \n"
"    mat4 m_trs  = u_bones[v_b[0]] * v_w[0];                                               \n"
"         m_trs += u_bones[v_b[1]] * v_w[1];                                               \n"
"         m_trs += u_bones[v_b[2]] * v_w[2];                                               \n"
"         m_trs += u_bones[v_b[3]] * v_w[3];                                               \n"
"    v_xyz = (m_trs * vec4(v_pos, 1.0f)).xyz;                                              \n"
"    v_str = (m_trs * vec4(v_nml, 0.0f)).xyz;                                              \n"
"}                                                                                         \n"
;

void cat::mesh_baker::create()
{
	const char* vars[] = {
		"v_xyz",
		"v_str",
	};
	_shd.begin();
	_shd.load(vs, cat::shader::SHADER_TYPE::VERTEX_SHADER);
	_shd.end(vars, 2);
	_loc_bones = _shd.getlocation("u_bones[0]");
}

void cat::mesh_baker::bake(const vao& ao, unsigned int vtsCount, const vbo& vts, const vbo& nml, const glm::mat4* mat, unsigned int count)
{
	if (count > 64) {
		std::cout << "cat::mesh_baker: warning: too many bones!" << std::endl;
		count = 64;
	}
	_shd.bind();
	for (unsigned int i = 0; i < count; i++) {
		_shd.setmat4(_loc_bones + i, mat[i]);
	}
	feedback::bind(vts, 0);
	feedback::bind(nml, 1);
	feedback::begin(cat::feedback::FB_TYPE::FB_POINTS);
	ao.bind();
	glDrawArrays(GL_POINTS, 0, vtsCount);
	feedback::end();
	feedback::unbind(nml);
	feedback::unbind(vts);
}

void cat::mesh_baker::bake(dynamic_mesh& ms, R3DBones& bones)
{
	bake(ms._secvao, ms._vtsCount, ms.mesh::_xyz, ms.mesh::_str, ms._currentState, bones.getCount());
}
