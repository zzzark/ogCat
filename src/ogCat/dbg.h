/*
//@author:	zrk
//@time:	2020 / 5 / 30
*/

#pragma once
#include "common.h"

#ifdef OGCAT_DBG_RELEASE
#define CAT_ASSERT(x) x;
#define CAT_CALL(x)  x;
#else
#define CAT_ASSERT(x) if(!(x)) { __debugbreak(); }
#define CAT_CALL(x) cat::ClearError();\
					x;\
					CAT_ASSERT(cat::ErrorLog(#x, __FILE__, __LINE__));
#endif


namespace cat {
	void ClearError();
	bool ErrorLog(const char* info, const char* srcfile, int line);
	class gui
	{
	public:
		static void Initialize(const char* glsl_version, GLFWwindow* window);
		static void BeginFrame();
		static void EndFrame();
		static void Terminate();
		static float GetFPS();
		void Begin(const char* windowName) const;
		void End() const;
		bool Button(const char* label) const;
		bool Checkbox(const char* label, bool* v) const;
		bool ColorEdit3(const char* label, float col[3]);
		bool ColorEdit4(const char* label, float col[4]);
		bool ListBox(const char* label, int* current_item, const char* const items[], int items_count);
		bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
		bool SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
		bool SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
		bool SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
	};
};