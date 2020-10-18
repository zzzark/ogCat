/*
//@author:	zrk
//@time:	2020 / 6/ 2
//example:
using namespace cat;
standardBuffer fb;
while (renderer::MainLoop()) {
	renderer::SetViewPort(0, 0, fb.getW(), fb.getH());	// set the right view port
	fb.bindForBoth();
	renderer::BeginFrame( R, G, B, A );
	{
		// draw something here
	}
	renderer::EndFrame(false); // do not swap buffer
	fb.unbindForBoth();
	renderer::SetViewPort(	// restore the view port
		renderer::GetInnerX(), 
		renderer::GetInnerY(), 
		renderer::GetInnerW(), 
		renderer::GetInnerH()
	);
	renderer::BeginFrame( R, G, B, A );
	{
		fb.blit(
			renderer::GetInnerX(), 
			renderer::GetInnerY(), 
			(renderer::GetInnerW() + 2 * renderer::GetInnerX()) / 2, 
			(renderer::GetInnerH() + 2 * renderer::GetInnerY()) / 2, 
			cat::TEXTURE_FILTER::TF_NEAREST
		);
		// draw gui here
	}
	renderer::EndFrame();
} // loop
*/

#pragma once
#include "texture2D.h"
#include "shader.h"
namespace cat
{
	/*
	//remarks:
	// vs: gl_Position = position;  (usually use it to draw a rectangle)
	// fs: do some effects here
	//
	*/
	class frameShader final : public shader
	{
		frameShader(const frameShader&) {}

		void begin() {};
		void load(const char* src, SHADER_TYPE type) {};
		void end() {};
	public:
		frameShader();
		virtual ~frameShader();

		// remarks:
		//  there is an output "vec2 v_uv" in vs (texCoord)
		//  and the rect is [-1, 1]x[-1, 1] and is scaled to [0, 1] in vs
		//
		void createFS(const char* fs);
	};

	/*
	//remarks:
	// with one color attachment and one depth attachment
	// 
	*/
	class standardBuffer final
	{
		unsigned int _id;
		texture2D _tex;
		texture2D _dep;
		standardBuffer(const standardBuffer&) { _id = 0; }
	public:
		standardBuffer();
		~standardBuffer();
		void create(unsigned int width, unsigned int height, bool bUseDepthBuffer, bool alphaChannel = false);
		
		void bindForDrawing();
		void bindForReading();
		void bindForBoth();
		void unbindForBoth();

		void blit(unsigned int winLeft, unsigned int winTop, unsigned int winRight, unsigned int winBottom, TEXTURE_FILTER filter);
		void release();

		inline const texture2D& texture() { return _tex; }
		inline const texture2D& depthbf() { return _dep; }
	};

	/*
	//remarks:
	// can be used to compute shadow map
	// 
	*/
	class depthBuffer final
	{
		unsigned int _id;
		texture2D _dep;
		depthBuffer(const depthBuffer&) { _id = 0; }
	public:
		depthBuffer();
		~depthBuffer();
		void create(unsigned int width, unsigned int height);

		void bindForDrawing();
		void bindForReading();
		void bindForBoth();
		void unbindForBoth();

		void release();
		
		inline const texture2D& texture() { return _dep; }
	};


	class frameBuffer final
	{
	public:
		static const unsigned int max_buffer_count = 8;
	private:
		unsigned int _id;
		unsigned int _index;
		unsigned int _count;
		texture2D _tex[frameBuffer::max_buffer_count];
		texture2D _dep;
		texture2D _stn;

		frameBuffer(const frameBuffer&) { _id = _index = _count = 0; }
	public:
		frameBuffer();
		~frameBuffer();

		// normal texture
		//  color value will be clamped to [0.0, 1.0]
		//
		void create(unsigned int width, unsigned int height, unsigned int count, bool bUseDepthBuffer = true, bool bUseStencilBuffer = false);
		
		// texture with specified color bit
		//  it can contain bigger range color value
		//   bit:     can be 32U, 16U
		//   channel: can be 1U, 2U, 3U, 4U
		void createEX(unsigned int width, unsigned int height, unsigned int count, unsigned int channel = 3, unsigned int bit = 32, bool bUseDepthBuffer = true, bool bUseStencilBuffer = false);

		// begin to create buffers
		//
		void begin();

		// create buffer with specific parameters
		//  must be called between begin() and end()
		//
		void createSpec(bool isDepthBuffer, bool isDepthStencil, unsigned int width, unsigned int height, unsigned int channel = 3, unsigned int bit = 32);

		// create buffer with user-defined texture
		// warning: the user-defined texture shouldn't be released
		//  must be called between begin() and end()
		//
		void createUserDefined(bool isDepth, bool isDepthStencil, unsigned int id);

		// 
		void end();

		// use buffers from 0 to [count - 1], 0 means not to use anything
		// 
		void switchBuffers(unsigned int count) const;

		// use specific buffer
		//
		void switchBuffer(unsigned int index) const;

		void bindForDrawing() const ;
		void bindForReading() const;
		void bindForBoth() const;
		void unbindForBoth() const;

		void release();
		inline const unsigned int getCount()           const { return _index; }
		inline const texture2D& getDep()               const { if (_stn.getID() == 0) return _dep; else return _stn; }
		inline const texture2D& getTex(unsigned index) const { return _tex[index]; }
		inline const texture2D& operator[](unsigned index) const { return getTex(index); }

		static unsigned int sys_getMaxBufferCount();
		static unsigned int sys_getDefinedBufferCount();
	};

}