#include <glfw3.h>
#include "ogCat/renderer.h"
#include "ogCat/gui.h"
#include "ogCat/camera.h"
#include "ogCat/frameBuffer.h"
#include "ogCat/gbuffer.h"
#include "ogCat/postprc.h"
#include "gmSys.h"
#include <iostream>
#include <chrono>

static void kb(int, int, int, int) {}
static void sc(double, double) {}
static void cp(double, double) {}
static void mp(int, int, int) {}

ogm::gmSys::fun_keybrd ogm::gmSys::_keybrd = kb;
ogm::gmSys::fun_scroll ogm::gmSys::_scroll = sc;
ogm::gmSys::fun_curpos ogm::gmSys::_curpos = cp;
ogm::gmSys::fun_mousep ogm::gmSys::_mousep = mp;

void ogm::gmSys::_updateTime()
{
	_lastTime = _nowTime;
	auto now = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now().time_since_epoch());
	_nowTime = now.count();
}

void ogm::gmSys::_lockCursor()
{
	if (_lock_cursor) {
		glfwSetCursorPos(cat::renderer::GetWindow(), cat::renderer::GetRealW() / 2, cat::renderer::GetRealH() / 2);
	}
}

void ogm::gmSys::initialize(
	unsigned int width, unsigned int height, const char* const caption,
	float zNear, float zFar, float fov,
	unsigned int max_fps, bool sizable, bool output_info)
{
	if (_is_created) return;
	_is_created = true;
	_updateTime();

	_width = width;
	_height = height;
	_zFar = zFar;
	_zNear = zNear;
	cat::renderer::Initialize(width, height, caption, sizable, 0, output_info);
	cat::gui::Initialize("#version 450", cat::renderer::GetWindow());

	_cam = new cat::camera;
	_cam->perspective(zNear, zFar, fov, (float)cat::renderer::GetOriginalW() / (float)cat::renderer::GetOriginalH());
	_cam->walk(glm::vec3(1, 1, 0));
	_cam->look(glm::vec3(0, 0, 0));

	cat::renderer::KeyCallback   (reinterpret_cast<GLFWkeyfun>        (_s_keybrd));
	cat::renderer::ScrollCallback(reinterpret_cast<GLFWscrollfun>     (_s_scroll));
	cat::renderer::CursorCallback(reinterpret_cast<GLFWcursorposfun>  (_s_curpos));
	cat::renderer::MouseCallback (reinterpret_cast<GLFWmousebuttonfun>(_s_mousep));

	unsigned sup = cat::frameBuffer::sys_getMaxBufferCount();
	unsigned req = cat::frameBuffer::sys_getDefinedBufferCount();
	if (output_info) {
		std::cout << "max MRT supported: " << sup << std::endl;
		std::cout << "max MRT required:  " << req << std::endl;
	}
	if (sup < req && output_info) {
		std::cout << "error: MRT not supported!" << std::endl;
	}
}

void ogm::gmSys::setFOV(float fov) const
{
	if (_cam)
		_cam->perspective(_zNear, _zFar, fov, (float)cat::renderer::GetOriginalW() / (float)cat::renderer::GetOriginalH());
}

void ogm::gmSys::terminate()
{
	if (_is_created) {
		_is_created = false;
		delete _cam; _cam = nullptr;
		cat::gui::Terminate();
		cat::renderer::Terminate();
	}
}

ogm::gmSys::~gmSys()
{
	terminate();
}

bool ogm::gmSys::shouldExitProgram() const
{
	return !cat::renderer::MainLoop() || _exit_flag;
}

void ogm::gmSys::exit()
{
	_exit_flag = true;
}

void ogm::gmSys::refresh()
{
	_updateTime();
	_lockCursor();
}

const glm::vec3& ogm::gmSys::get_eye() const
{
	static const glm::vec3 tmp(0);
	if (_cam) {
		return _cam->getEye();
	}
	return tmp;
}

const glm::vec3& ogm::gmSys::get_at() const
{
	static const glm::vec3 tmp(0);
	if (_cam) {
		return _cam->getAt();
	}
	return tmp;
}

void ogm::gmSys::set_eye(const glm::vec3& eye) const
{
	if (_cam) {
		((cat::camera*)_cam)->walk(eye);
	}
}

void ogm::gmSys::set_at(const glm::vec3& at) const
{
	if (_cam) {
		((cat::camera*)_cam)->look(at);
	}
}

unsigned ogm::gmSys::getRealW() const
{
	return cat::renderer::GetRealW();
}

unsigned ogm::gmSys::getRealH() const
{
	return cat::renderer::GetRealH();
}

void ogm::gmSys::_s_keybrd(void*, int a, int b, int c, int d)
{
	_keybrd(a, b, c, d);
}

void ogm::gmSys::_s_scroll(void*, double a, double b)
{
	_scroll(a, b);
}

void ogm::gmSys::_s_curpos(void*, double a, double b)
{
	_curpos(a, b);
}

void ogm::gmSys::_s_mousep(void*, int a, int b, int c)
{
	_mousep(a, b, c);
}

void ogm::gmSys::setKeybrdCallback(fun_keybrd f)
{
	_keybrd = f;
}

void ogm::gmSys::setScrollCallback(fun_scroll f)
{
	_scroll = f;
}

void ogm::gmSys::setCurposCallback(fun_curpos f)
{
	_curpos = f;
}

void ogm::gmSys::setMousepCallback(fun_mousep f)
{
	_mousep = f;
}

void ogm::gmSys::lockCursor(bool opts)
{
	_lock_cursor = opts;
}

void ogm::gmSurface::initialize()
{
	if (_is_created == false) {
		_gbuf = new cat::gbuffer;
		_gbuf->create(cat::renderer::GetOriginalW(), cat::renderer::GetOriginalH());
		_shdBuf = new cat::shadowBuffer;
		_shdBuf->create(cat::renderer::GetOriginalW(), cat::renderer::GetOriginalH());
		_pstprc = new cat::postprc;
		_pstprc->create(*_gbuf);
		_is_created = true;
	}
}

void ogm::gmSurface::terminate()
{
	if (_is_created) {
		delete _gbuf; _gbuf = nullptr;
		delete _pstprc; _pstprc = nullptr;
		_is_created = false;
	}
}

ogm::gmSurface::~gmSurface()
{
	terminate();
}

void ogm::gmSurface::prepare() const
{
	cat::renderer::SetViewPort(0, 0, cat::renderer::GetOriginalW(), cat::renderer::GetOriginalH());
	if (_gbuf) {
		_gbuf->bind();
		_gbuf->switchBuffers(cat::gbuffer::MAX_BUFFER_LAYER_COUNT);
		cat::renderer::ClearAll();
		_gbuf->unbind();
	}
}

void ogm::gmSurface::begin_rendering() const
{
	if (_gbuf)
		_gbuf->bind();
}

void ogm::gmSurface::end_rendering() const
{
	if (_gbuf)
		_gbuf->unbind();
}

void ogm::gmSurface::begin_process() const
{
	cat::renderer::SetViewPort(0, 0, cat::renderer::GetOriginalW(), cat::renderer::GetOriginalH());
	if (_gbuf && _shdBuf && _pstprc) {
		_shdBuf->applyEffect(*_gbuf);
		_pstprc->begin();
	}
}

void ogm::gmSurface::process(const gmSys& sys) const
{
	if (_gbuf && _shdBuf && _pstprc) {
		_pstprc->process(*_gbuf, *_shdBuf, *(sys.cameraPointer()));
	}
}

void ogm::gmSurface::end_process() const
{
	if (_gbuf && _shdBuf && _pstprc) {
		_pstprc->end();
	}
}

void ogm::gmSurface::begin_shadow() const
{
	cat::renderer::SetViewPort(0, 0, cat::renderer::GetOriginalW(), cat::renderer::GetOriginalH());
	if (_shdBuf) {
		_shdBuf->begin();
	}
}

void ogm::gmSurface::end_shadow() const
{
	if (_shdBuf) {
		_shdBuf->end();
	}
}

void ogm::gmSurface::setBlurFactor(float factor) const
{
	if (_shdBuf)
		_shdBuf->setScaleFactor(factor);
}
