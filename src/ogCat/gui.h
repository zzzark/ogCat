#pragma once
#ifndef _glfw3_h_
struct GLFWwindow;
typedef void (*GLFWframebuffersizefun)(GLFWwindow*, int, int);
typedef void (*GLFWkeyfun)(GLFWwindow*, int, int, int, int);
typedef void (*GLFWmousebuttonfun)(GLFWwindow*, int, int, int);
typedef void (*GLFWcursorposfun)(GLFWwindow*, double, double);
typedef void (*GLFWscrollfun)(GLFWwindow*, double, double);
#endif

namespace cat
{
	class gui
	{
	public:
		static void Initialize(const char* glsl_version, GLFWwindow* window);
		static void BeginFrame();
		static void EndFrame();
		static void Terminate();
		static float GetFPS();
		void Begin(const char* windowName, bool collapsed = false, float text_size = 1.0f) const;
		void End() const;
		bool Button(const char* label) const;
		bool Checkbox(const char* label, bool* v) const;
		bool ColorEdit3(const char* label, float col[3]) const;
		bool ColorEdit4(const char* label, float col[4]) const;
		bool ListBox(const char* label, int* current_item, const char* const items[], int items_count) const;
		bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f) const;
		bool SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f) const;
		bool SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f) const;
		bool SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f) const;
	};
}
