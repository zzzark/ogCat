/*
//@author:	zark
//@time:	2020 / 5 / 30
//example:

#include <Renderer.h>
using cat::Renderer;
int main()
{
	Renderer::Initialize(800, 600, "ogCat demo", true, 0, true);
	while (Renderer::MainLoop()) {
		Renderer::BeginFrame(0.5f, 1, 1, 1);
		Renderer::EndFrame();
	}
	Renderer::Terminate();
}
*/

#pragma once

#ifndef _glfw3_h_
struct GLFWwindow;
typedef void (*GLFWframebuffersizefun)(GLFWwindow*, int, int);
typedef void (*GLFWkeyfun)(GLFWwindow*, int, int, int, int);
typedef void (*GLFWmousebuttonfun)(GLFWwindow*, int, int, int);
typedef void (*GLFWcursorposfun)(GLFWwindow*, double, double);
typedef void (*GLFWscrollfun)(GLFWwindow*, double, double);
#endif
namespace cat {
	class renderer
	{
		static unsigned int _originalW, _originalH;
		static unsigned int _innerX, _innerY, _innerW, _innerH;
		static unsigned int _realW, _realH;
		static GLFWwindow* _window;
	public:
		static void Initialize(int width, int height, const char* caption, bool sizeable = true, int multisample = 0, bool outputInfo = true);
		static void Terminate();
		static void BeginFrame(float r, float g, float b, float a);
		static void EndFrame(bool swap = true);

		static bool MainLoop();
		static void SetFPS(unsigned int fps);
		static void SetViewPort(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
		static void SetClearColor(float r, float g, float b, float a);
		static void SetClearDepth(float d);
		static void SetClearStencil(int s);
		static void ClearColor();
		static void ClearDepth();
		static void ClearStencil();
		static void ClearAll();

		static void DisableCullFace();
		static void EnableCullFace();
		static void CullFaceFront();
		static void CullFaceBack();
		static void LineMode();
		static void SolidMode();

		static GLFWframebuffersizefun	ResizeCallback(GLFWframebuffersizefun fun);
		static GLFWkeyfun				KeyCallback(GLFWkeyfun fun);
		static GLFWmousebuttonfun		MouseCallback(GLFWmousebuttonfun fun);
		static GLFWcursorposfun			CursorCallback(GLFWcursorposfun fun);
		static GLFWscrollfun			ScrollCallback(GLFWscrollfun fun);

		static inline GLFWwindow* GetWindow() { return _window; }
		static inline unsigned int GetOriginalW() { return _originalW; }
		static inline unsigned int GetOriginalH() { return _originalH; }
		static inline unsigned int GetRealW() { return _realW; }
		static inline unsigned int GetRealH() { return _realH; }
		static inline unsigned int GetInnerW() { return _innerW; }
		static inline unsigned int GetInnerH() { return _innerH; }
		static inline unsigned int GetInnerX() { return _innerX; }
		static inline unsigned int GetInnerY() { return _innerY; }

		// this function must be called if you resized the window
		// e.g.
		//  fun(windows, w, h) {
		//    renderer::UpdateResizedWindow(0, 0, w, h);
		//  }
		static void UpdateResizedWindow(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

		// this function is used to set the window view port
		static void SetWindow(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
	};
};