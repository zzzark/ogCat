#include "3DObject.h"
#include "mesh_shader.h"
#include "main_initializor.h"
#include "postprc.h"
#include <thread>
#include <string>

using glm::vec3;
using glm::vec4;
using glm::mat4;

int main()
{
	renderer::Initialize(1200, 800, "opengl demo", false, 0, true);
	gui::Initialize("#version 450", renderer::GetWindow());
	main_initialize();
	{
		/* ---- g-buffer ---- */
		gbuffer gbuf;
		gbuf.create(renderer::GetOriginalW(), renderer::GetOriginalH());

		texture2D tex;
		tex.createFromFile("res/textures/boxman.jpg", true);

		material mtl = { vec3(0,0,0), vec3(.5f,.5f,.5f), vec3(.5f,.5f,.5f), 5};

		R3DBones bon;
		bon.load("res/models/boxman.bon");
		R3DAnimation ani;
		ani.load("res/models/boxman.ani", bon);
		dynamic_mesh dms;
		dms.load("res/models/boxman.bin", bon);

		mesh_shader mshd;
		mshd.create();

		terrainBlock tnb;
		tnb.load("res/textures/terrain.jpg", 0.1f, 1.0f, 0.1f, -50.0f, -50.0f, 0.0f);

		/* ---- oven ---- */
		mesh_baker baker;
		baker.create();

		///* ---- shadow ---- */
		shadowBuffer shdBuffer;
		shdBuffer.create(renderer::GetOriginalW(), renderer::GetOriginalH(), 16, 1.0f);
		shdBuffer.setScaleFactor(1.0f);

		/* ---- directional light ---- */
		const float SM_ORTHO_SIZE = 10.0f;
		const unsigned int SM_SIZE = 1024;
		camera lit_cam;
		lit_cam.ortho(-SM_ORTHO_SIZE, SM_ORTHO_SIZE, -SM_ORTHO_SIZE, SM_ORTHO_SIZE, -SM_ORTHO_SIZE, SM_ORTHO_SIZE);
		lit_cam.walk(glm::vec3(1, 1, 1));
		lit_cam.look(glm::vec3(0, 0, 0));
		directionalLight DL;
		DL.ambient = glm::vec3(0.1f, 0.1f, 0.15f);
		DL.color = glm::vec3(0.25f, 0.25f, 0.85f);
		DL.direction = glm::normalize(lit_cam.getAt() - lit_cam.getEye());
		DL.intensity = 1.0f;

		shadowMap2D shd2D;
		shd2D.create(SM_SIZE, SM_SIZE);

		shadow2DEffect effect_sha2D;
		effect_sha2D.create();
		effect_sha2D.setBias(0.002f);
		effect_sha2D.setSize((float)SM_SIZE);

		/* ---- point light ---- */
		const unsigned int P_SIZE = 1024;
		pointLight PL;
		PL.position = glm::vec3(0, 2.5f, -7.0f);
		PL.ambient = glm::vec3(0.1f, 0.1f, 0.0f);
		PL.color = glm::vec3(0.8f, 0.8f, 0.6f);
		PL.intensity = 5.0f;
		const float PL_zNear = 0.1f;
		const float PL_zFar = 1000.0f;

		shadowMapCUBE shdCB;
		shdCB.create(P_SIZE, P_SIZE);

		shadowCUBEEffect effect_shaCB;
		effect_shaCB.create();
		effect_shaCB.setFarPlane(PL_zFar);
		effect_shaCB.setMaxBrightness(5.0f);


		postprc pstp;
		pstp.create(gbuf);

		// ----------------------------------------------------------------------------------------------------- //
		unsigned int tm = 0;
		while (renderer::MainLoop())
		{
			handle_events();

			dms.update(ani, 200, mat4(1));
			baker.bake(dms, bon);

			gbuf.bind();
			mesh_shader::prepareBuffer(gbuf);
			mshd.bind();
			mshd.draw(tex, mtl, vec3(1, 0, 0), g_cam, mat4(1), dms);
			mshd.draw(tex, mtl, vec3(2, 0, 0), g_cam, mat4(1), tnb);
			mshd.unbind();
			gbuf.unbind();

			/* ---- directional light ---- */
			renderer::SetViewPort(0, 0, SM_SIZE, SM_SIZE);
			shd2D.begin();
			{
				shd2D.draw(lit_cam, glm::mat4(1.0f), dms);
				shd2D.draw(lit_cam, glm::mat4(1.0f), tnb);
			}
			shd2D.end();

			/* ---- point light ---- */
			renderer::SetViewPort(0, 0, P_SIZE, P_SIZE);
			shdCB.begin(PL.position, PL_zNear, PL_zFar);
			{
				shdCB.draw(glm::mat4(1.0f), dms);
				shdCB.draw(glm::mat4(1.0f), tnb);
			}
			shdCB.end();

			/* ---- shadow buffer ---- */
			renderer::SetViewPort(0, 0, gbuf.getWidth(), gbuf.getHeight());
			shdBuffer.begin();
			{
				effect_sha2D.applyEffect(shdBuffer, shd2D, gbuf, DL, lit_cam, g_cam);
				effect_shaCB.applyEffect(shdBuffer, shdCB, gbuf, PL, g_cam);
			}
			shdBuffer.end();

			/* ---- soft shadow ---- */
			renderer::SetViewPort(0, 0, gbuf.getWidth(), gbuf.getHeight());
			shdBuffer.applyEffect(gbuf);

			/* ---- post processing ---- */
			pstp.process(gbuf, shdBuffer, g_cam);
		} // loop
	}// initialize
	gui::Terminate();
	renderer::Terminate();
}
