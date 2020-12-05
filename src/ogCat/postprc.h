/*
@author:	zrk
@date:		2020/7/11
*/
#pragma once
#include "frameEffect.h"
#include "frameBuffer.h"
#include "shadowBuffer.h"
#include "camera.h"
namespace cat
{
	class postprc
	{
	protected:
		frameEffect _eff_bloom_depart;

		frameEffect _eff_bloom_blur_V;
		frameEffect _eff_bloom_blur_H;
		frameEffect _eff_hdr_combine;
		frameEffect _eff_fxaa;
		//frameEffect _eff_depth_blur;
		//frameEffect _eff_motionBlur;


		frameBuffer _bloom;
		frameBuffer _tmpbuf;
		//frameBuffer _final[2];

		int _loc_last_mvp = -1;
		int _loc_depth_eye = -1;
		//int _loc_depth_dir = -1;
		//int _loc_depth_len = -1;
		glm::mat4 _last_mvp = glm::mat4(1);
	public:
		void create(const gbuffer& gbf);
		void process(const gbuffer& gbf, shadowBuffer& sbf, camera& org);
		void begin() const;
		void end() const;
	};
}
