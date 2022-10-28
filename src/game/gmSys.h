/*
@author:  zrk
@date:    2020/10/18
*/
#pragma once
#include "glm/glm.hpp"

namespace cat
{
	class camera;
	class gbuffer;
	class shadowBuffer;
	class postprc;
}

namespace ogm
{
	class gmSys final
	{
	private:
		static void _s_keybrd(void*, int, int, int, int);
		static void _s_scroll(void*, double, double);
		static void _s_curpos(void*, double, double);
		static void _s_mousep(void*, int, int, int);
	private:
		bool _is_created = false;
		unsigned long long _lastTime = 0;
		unsigned long long _nowTime = 0;
	public:
		typedef void (*fun_keybrd)(int key, int scancode, int action, int modes);
		typedef void (*fun_scroll)(double xoff, double yoff);
		typedef void (*fun_curpos)(double x, double y);
		typedef void (*fun_mousep)(int a, int b, int c);
	private:
		static fun_keybrd _keybrd;
		static fun_scroll _scroll;
		static fun_curpos _curpos;
		static fun_mousep _mousep;
		
		unsigned _width = 0;
		unsigned _height = 0;
		float _zFar  = 0.0f;
		float _zNear = 0.0f;

		bool _exit_flag = false;
		bool _lock_cursor = false;
		
		cat::camera* _cam = nullptr;
	private:
		void _updateTime();
		void _lockCursor();
	public:
		void initialize(
			unsigned int width, unsigned int height, const char* const caption,
			float zNear = 0.1f, float zFar = 1000.0f, float fov = 0.8f,
			unsigned int max_fps = 60, bool sizable = false, bool output_info = false);
		void terminate();
		~gmSys();

		/* ---- control callback functions ---- */
		void setKeybrdCallback(fun_keybrd f);
		void setScrollCallback(fun_scroll f);
		void setCurposCallback(fun_curpos f);
		void setMousepCallback(fun_mousep f);


		/* ---- system ---- */
		void exit();
		void refresh();
		bool shouldExitProgram() const;


		/* ---- interaction ---- */
		// if opts is true: 
		//    cursor will be set to (getRealW()/2, getRealH()/2) when calling method refresh()
		void lockCursor(bool opts);


		/* ---- time ---- */
		unsigned long long getTimeDelta() const { return _nowTime - _lastTime; }
		unsigned long long getNowTime() const { return _nowTime; }


		/* ---- camera ---- */
		void setFOV(float fov) const;
		float getFarPlane()  const { return _zFar; }
		float getNearPlane() const { return _zNear; }
		glm::vec3 get_eye() const;
		glm::vec3 get_at()  const;
		void set_eye(const glm::vec3& eye) const;
		void set_at (const glm::vec3& at)  const;


		/* ---- system settings ---- */
		unsigned getWidth()  const { return _width; }
		unsigned getHeight() const { return _height; }
		unsigned getRealW() const;
		unsigned getRealH() const;

		/* ---- pointers ---- */
		cat::camera* cameraPointer() const { return _cam; }
	};

	class gmSurface final
	{
	private:
		bool _is_created = false;
	private:
		cat::gbuffer* _gbuf = nullptr;
		cat::shadowBuffer* _shdBuf = nullptr;
		cat::postprc* _pstprc = nullptr;
	public:
		void initialize();
		void terminate();
		~gmSurface();
		void prepare() const;

		void begin_rendering() const;
		void end_rendering() const;

		void begin_process() const;
		void process(const gmSys& sys) const;
		void end_process() const;

		void begin_shadow() const;
		void end_shadow() const;

		void setBlurFactor(float factor) const;

		cat::gbuffer* gbufferPointer() const { return _gbuf; }
		cat::shadowBuffer* shadowBufferPointer() const { return _shdBuf; }
	};

}