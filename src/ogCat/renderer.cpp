#include "dbg.h"
#include "renderer.h"
using cat::renderer;

unsigned int renderer::_originalW = 0;
unsigned int renderer::_originalH = 0;
unsigned int renderer::_realH = 0;
unsigned int renderer::_realW = 0;
unsigned int renderer::_innerX = 0;
unsigned int renderer::_innerY = 0;
unsigned int renderer::_innerW = 0;
unsigned int renderer::_innerH = 0;
GLFWwindow* renderer::_window = nullptr;

static void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	renderer::UpdateResizedWindow(0, 0, width, height);
	unsigned int w = renderer::GetOriginalW();
	unsigned int h = renderer::GetOriginalH();
	if (w != 0 && h != 0) {
		//glViewport(0, 0, width, height);
		double b = (double)w / (double)h;
		double a = (double)width / (double)height;
		if (a > b) {	// wider
			w = (unsigned int)((double)height * b);
			h = height;
			renderer::SetWindow(std::abs(width - (int)w) / 2, 0, w, h);
		}
		else {
			w = width;
			h = (unsigned int)((double)width / b);
			renderer::SetWindow(0, std::abs(height - (int)h) / 2, w, h);
		}
	}
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	return;
}
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	return;
}
static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	return;
}
static void scroll_callback(GLFWwindow* window, double x, double y)
{
	return;
}

void cat::renderer::UpdateResizedWindow(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	_realW = w;
	_realH = h;
}

void cat::renderer::SetWindow(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	_innerX = x;
	_innerY = y;
	_innerW = w;
	_innerH = h;
	glViewport(x, y, w, h);
}
void renderer::Initialize(int width, int height, const char* caption, bool sizeable, int multisample, bool outputInfo)
{
	if (!glfwInit()) {
		std::cout << "glfw initialization error!" << std::endl;
		CAT_ASSERT(false);
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, sizeable ? GLFW_TRUE : GLFW_FALSE);

	_window = glfwCreateWindow(width, height, caption, NULL, NULL);
	if (!_window) {
		glfwTerminate();
		std::cout << "An error occur while glfw creating window!" << std::endl;
		CAT_ASSERT(false);
		return;
	}
	glfwMakeContextCurrent(_window);
	glfwSwapInterval(1);	// v-sync
	glfwSetFramebufferSizeCallback(_window, framebuffer_resize_callback);

	if (glewInit() != GLEW_OK) {
		std::cout << "glew initialization error!" << std::endl;
		CAT_ASSERT(false);
		return;
	}
	_innerX = _innerY = 0;
	_innerW = _realW = _originalW = width;
	_innerH = _realH = _originalH = height;
	if (outputInfo) {
		const GLubyte* name = glGetString(GL_VENDOR);
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* version = glGetString(GL_VERSION);
		const GLubyte* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
		std::cout << "vendor:      " << name << std::endl;
		std::cout << "renderer:    " << renderer << std::endl;
		std::cout << "version:     " << version << std::endl;
		std::cout << "glsl:        " << glsl << std::endl;
	}
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	if (multisample > 0) {
		glfwWindowHint(GLFW_SAMPLES, multisample);
		glEnable(GL_MULTISAMPLE);
	}
	else {
		glDisable(GL_MULTISAMPLE);
	}

}

void renderer::Terminate()
{
	glfwTerminate();
}


void renderer::BeginFrame(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void cat::renderer::ClearAll()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void cat::renderer::DisableCullFace()
{
	glDisable(GL_CULL_FACE);
}

void cat::renderer::EnableCullFace()
{
	glEnable(GL_CULL_FACE);
}

void cat::renderer::CullFaceFront()
{
	glCullFace(GL_FRONT);
}

void cat::renderer::CullFaceBack()
{
	glCullFace(GL_BACK);
}

void cat::renderer::LineMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void cat::renderer::SolidMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void renderer::EndFrame(bool swap)
{
	if (swap) {
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}
}

bool renderer::MainLoop()
{
	return (!glfwWindowShouldClose(_window));
}

void renderer::SetFPS(unsigned int fps)
{
	if (fps == 0 || fps > 60) {
		glfwSwapInterval(0);
	}
	else {
		glfwSwapInterval(60 / fps);
	}
}

void cat::renderer::SetViewPort(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
	glViewport(x, y, w, h);
}

void cat::renderer::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void cat::renderer::SetClearDepth(float d)
{
	glClearDepthf(d);
}

void cat::renderer::SetClearStencil(int s)
{
	glClearStencil(s);
}

void cat::renderer::ClearColor()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void cat::renderer::ClearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void cat::renderer::ClearStencil()
{
	glClear(GL_STENCIL_BUFFER_BIT);
}

GLFWframebuffersizefun renderer::ResizeCallback(GLFWframebuffersizefun fun)
{
	static auto last = framebuffer_resize_callback;
	glfwSetFramebufferSizeCallback(_window, fun);
	auto l = last;
	last = fun;
	return l;
}

GLFWkeyfun renderer::KeyCallback(GLFWkeyfun fun)
{
	static auto last = key_callback;
	glfwSetKeyCallback(_window, fun);
	auto l = last;
	last = fun;
	return l;
}

GLFWmousebuttonfun renderer::MouseCallback(GLFWmousebuttonfun fun)
{
	static auto last = mouse_button_callback;
	glfwSetMouseButtonCallback(_window, fun);
	auto l = last;
	last = fun;
	return l;
}

GLFWcursorposfun renderer::CursorCallback(GLFWcursorposfun fun)
{
	static auto last = cursor_position_callback;
	glfwSetCursorPosCallback(_window, fun);
	auto l = last;
	last = fun;
	return l;
}

GLFWscrollfun renderer::ScrollCallback(GLFWscrollfun fun)
{
	static auto last = scroll_callback;
	glfwSetScrollCallback(_window, fun);
	auto l = last;
	last = fun;
	return l;
}
