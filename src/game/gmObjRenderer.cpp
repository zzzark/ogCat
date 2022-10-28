#include "gmObjRenderer.h"
#include "ogCat/mesh_instance.h"
#include "ogCat/dynamic_mesh.h"
#include "ogCat/mesh_shader.h"

void ogm::gmObjRenderer::initialize()
{
	if (_is_created) return;
	_is_created = true;
	_baker = new cat::mesh_baker;
	_shader = new cat::defaultMeshShader;	// use the default mesh shader
	_baker->create();
	_shader->create();
}

void ogm::gmObjRenderer::terminate()
{
	if (_is_created) {
		_is_created = false;
		delete _baker; _baker = nullptr;
		delete _shader; _shader = nullptr;
	}
}

ogm::gmObjRenderer::~gmObjRenderer()
{
	terminate();
}

void ogm::gmObjRenderer::begin(gmSurface& suf) const
{
	cat::renderer::SetViewPort(0, 0, cat::renderer::GetOriginalW(), cat::renderer::GetOriginalH());
	if (_shader) {
		_shader->prepareBuffer(*(suf.gbufferPointer()));
		_shader->bind();
	}
}

void ogm::gmObjRenderer::end() const
{
	if (_shader) {
		_shader->unbind();
	}
}

void ogm::gmObjRenderer::render(gmObj& obj, gmSurface& suf, gmSys& sys)
{
	if (!_baker || ! _shader) return;
	auto inst = obj.meshInstancePointer();
	auto bone = obj.bonesPointer();
	auto anim = obj.animPointer();
	if (bone && anim) {
		auto dms = inst->getDynamicMesh();
		dms->update(*anim, (unsigned int)sys.getNowTime(), glm::mat4(1.0f));
		_baker->bake(*dms, *bone);
	}
	if (inst && inst->getMesh()) {
		_shader->draw(inst->getTex(), inst->mtl, inst->rec, *(sys.cameraPointer()), inst->mdl, *(inst->getMesh()));
	}
}

void ogm::gmObjRenderer::render(
	cat::mesh* ms, cat::texture2D* tex,
	const cat::material* mtl, const glm::mat4& mat,
	const glm::vec3& rec, gmSurface& suf, gmSys& sys)
{
	if (!_baker || !_shader || !tex || !ms) return;
	_shader->draw(*tex, *mtl, rec, *(sys.cameraPointer()), mat, *ms);

}
