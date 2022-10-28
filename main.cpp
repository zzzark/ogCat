#include "game/gmSys.h"
#include "game/gmObject.h"
#include "game/gmObjRenderer.h"
#include "game/gmLight.h"
#include "game/gmWorld.h"
#include "game/gmParticle.h"
#include "glm/gtc/matrix_transform.hpp"
#include "main_controller.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

ogm::gmSys     g_gmsys;
ogm::gmSurface g_gmsuf;

int main()
{
	g_gmsys.initialize(1200, 800, "2019151030 郑瑞坤");
	g_gmsys.setCurposCallback(ctrl_cursor);
	g_gmsys.setKeybrdCallback(ctrl_key);
	g_gmsys.setScrollCallback(ctrl_scroll);
	g_gmsuf.initialize();

	ogm::gmObjRenderer objRenderer;
	objRenderer.initialize();

	ogm::gmSky sky1;
	ogm::gmSky sky2;
	sky1.create("res/textures/s1/1.jpg", "res/textures/s1/2.jpg", "res/textures/s1/3.jpg",
				"res/textures/s1/4.jpg", "res/textures/s1/5.jpg", "res/textures/s1/6.jpg");
	sky2.create("res/textures/s2/1.jpg", "res/textures/s2/2.jpg", "res/textures/s2/3.jpg",
				"res/textures/s2/4.jpg", "res/textures/s2/5.jpg", "res/textures/s2/6.jpg");
	
	ogm::gmPointLight pntLight;
	ogm::gmDirectionalLight dirLight;
	pntLight.create();
	dirLight.create();
	dirLight.setRGB(.1f, .1f, .5f);
	pntLight.setRGB(13, 13, 6);
	
	ogm::gmObj obj_boxman;
	ogm::gmObj obj_ground;
	obj_boxman.createFromFile("res/objects/boxman.txt");
	obj_ground.createFromFile("res/objects/ground.txt");

	ogm::gmDust dust;
	dust.createDust("res/textures/rain.jpg", 1000, 5, 5, .3f);
	dust.setSize(0.01f);
	dust.setIntensity(10.0f);
	dust.setRate(0.1f);

	ogm::gmFireworks fireworks;
	fireworks.createFireworks("res/textures/rain.jpg", 1000, glm::vec3(0), 3, 0.05f);
	fireworks.setSize(0.01f);
	fireworks.setIntensity(10.0f);
	fireworks.setRate(1.0f);

	while (!g_gmsys.shouldExitProgram())
	{
		// ---- handle evnets ---- //
		float dx, dy, dz, dt, dp;
		ctrl_handle_events();
		ctrl_get_deltas(dx, dy, dz, dt, dp);

		auto dir = glm::normalize(g_gmsys.get_at() - g_gmsys.get_eye());
		auto dxz = glm::normalize(glm::vec3(dir.x, 0, dir.z));
		auto pos = g_gmsys.get_eye() + 2.0f * dxz;
		pos.y = 0;
		auto theta = glm::dot(glm::vec3(0, 0, 1), dxz);
		obj_boxman.get_model_matrix() = glm::rotate(glm::mat4(1), theta, glm::vec3(0, 1, 0));
		obj_boxman.moveTo(pos);
		

		// ---- before rendering ---- //
		g_gmsys.refresh();
		g_gmsuf.prepare();

		// ---- render objects ---- //
		g_gmsuf.begin_rendering();
		
		
		objRenderer.begin(g_gmsuf);
 		objRenderer.render(obj_boxman, g_gmsuf, g_gmsys);
		objRenderer.render(obj_ground, g_gmsuf, g_gmsys);
		objRenderer.end();

		sky1.renderTheSkey(g_gmsys, g_gmsuf);
		sky2.renderTheSkey(g_gmsys, g_gmsuf);

		dust.render(g_gmsuf, g_gmsys);
		fireworks.render(g_gmsuf, g_gmsys);
		

		g_gmsuf.end_rendering();

		// ---- render shadows ---- //
		pntLight.begin();
		pntLight.draw(obj_boxman);
		pntLight.draw(obj_ground);
		pntLight.end();

		dirLight.begin();
		dirLight.draw(obj_boxman);
		dirLight.draw(obj_ground);
		dirLight.end();

		// ---- apply shadows ---- //
		g_gmsuf.begin_shadow();
		dirLight.apply(g_gmsys, g_gmsuf);
		pntLight.apply(g_gmsys, g_gmsuf);
		g_gmsuf.end_shadow();


		// ---- apply effects ---- //
		g_gmsuf.begin_process();
		g_gmsuf.process(g_gmsys);
		g_gmsuf.end_process();

	}
	return 0;
}



//int dfsdf ()
//{
//	ogm::gmSys gmsys;
//	gmsys.initialize(1200, 800, "ogCat demo");
//	gmsys.setCurposCallback(ctrl_cursor);
//	gmsys.setKeybrdCallback(ctrl_key);
//	gmsys.setScrollCallback(ctrl_scroll);
//	{
//		auto mdlMatrix = glm::mat4(1.0f);
//		mdlMatrix[3][0] = 1;
//		mdlMatrix[3][1] = 1;
//		mdlMatrix[3][2] = 1;
//		/* ---- g-buffer ---- */
//		gbuffer gbuf;
//		gbuf.create(renderer::GetOriginalW(), renderer::GetOriginalH());
//
//		texture2D tex;
//		tex.createFromFile("res/textures/boxman.jpg", true);
//
//		material mtl = { vec3(0,0,0), vec3(.5f,.5f,.5f), vec3(.5f,.5f,.5f), 5};
//
//		R3DBones bon;
//		bon.load("res/models/boxman.bon");
//		R3DAnimation ani;
//		ani.load("res/models/boxman.ani", bon);
//		dynamic_mesh dms;
//		dms.load("res/models/boxman.bin", bon);
//
//		defaultMeshShader mshd;
//		mshd.create();
//
//		terrainBlock tnb;
//		tnb.load("res/textures/terrain.jpg", 0.1f, 1.0f, 0.1f, -50.0f, -50.0f, 0.0f);
//
//		/* ---- oven ---- */
//		mesh_baker baker;
//		baker.create();
//
//		///* ---- shadow ---- */
//		shadowBuffer shdBuffer;
//		shdBuffer.create(renderer::GetOriginalW(), renderer::GetOriginalH(), 16, 1.0f);
//		shdBuffer.setScaleFactor(1.0f);
//
//		/* ---- directional light ---- */
//		const float SM_ORTHO_SIZE = 10.0f;
//		const unsigned int SM_SIZE = 1024;
//		camera lit_cam;
//		lit_cam.ortho(-SM_ORTHO_SIZE, SM_ORTHO_SIZE, -SM_ORTHO_SIZE, SM_ORTHO_SIZE, -SM_ORTHO_SIZE, SM_ORTHO_SIZE);
//		lit_cam.walk(glm::vec3(1, 1, 1));
//		lit_cam.look(glm::vec3(0, 0, 0));
//		directionalLight DL;
//		DL.ambient = glm::vec3(0.1f, 0.1f, 0.15f);
//		DL.color = glm::vec3(0.25f, 0.25f, 0.85f);
//		DL.direction = glm::normalize(lit_cam.getAt() - lit_cam.getEye());
//		DL.intensity = 1.0f;
//
//		shadowMap2D shd2D;
//		shd2D.create(SM_SIZE, SM_SIZE);
//
//		shadow2DEffect effect_sha2D;
//		effect_sha2D.create();
//		effect_sha2D.setBias(0.002f);
//		effect_sha2D.setSize((float)SM_SIZE);
//
//		/* ---- point light ---- */
//		const unsigned int P_SIZE = 1024;
//		pointLight PL;
//		PL.position = glm::vec3(0, 5.0f, 0.0f);
//		PL.ambient = glm::vec3(0.1f, 0.1f, 0.0f);
//		PL.color = glm::vec3(10, 10, 10);
//		PL.intensity = 50.0f;
//		const float PL_zNear = 0.1f;
//		const float PL_zFar = 1000.0f;
//
//		shadowMapCUBE shdCB;
//		shdCB.create(P_SIZE, P_SIZE);
//
//		shadowCUBEEffect effect_shaCB;
//		effect_shaCB.create();
//		effect_shaCB.setFarPlane(PL_zFar);
//		effect_shaCB.setMaxBrightness(10.0f);
//
//
//		postprc pstp;
//		pstp.create(gbuf);
//
//		// ----------------------------------------------------------------------------------------------------- //
//		unsigned int tm = 0;
//		while (!gmsys.shouldExitProgram())
//		{
//			handle_events();
//			gmsys.set_eye(g_cam.getEye() + vec3(0, 1, 0));
//			gmsys.set_at(g_cam.getAt() + vec3(0, 1, 0));
//
//			dms.update(ani, tm+=16, mat4(1));
//			baker.bake(dms, bon);
//
//			gbuf.bind();
//			renderer::ClearAll();
//			mshd.prepareBuffer(gbuf);
//			mshd.bind();
//			mshd.draw(tex, mtl, vec3(1, 0, 0), *gmsys.cameraPointer(), mdlMatrix, dms);
//			mshd.draw(tex, mtl, vec3(2, 0, 0), *gmsys.cameraPointer(), mat4(1), tnb);
//			mshd.unbind();
//			gbuf.unbind();
//
//			/* ---- directional light ---- */
//			//renderer::SetViewPort(0, 0, SM_SIZE, SM_SIZE);
//			//shd2D.begin();
//			//{
//			//	shd2D.draw(lit_cam, mdlMatrix, dms);
//			//	shd2D.draw(lit_cam, glm::mat4(1.0f), tnb);
//			//}
//			//shd2D.end();
//
//			/* ---- point light ---- */
//			static float tm = 0.0f;
//			tm += 0.05f;
//			PL.position = glm::vec3(3.0f * cos(tm), 3.0f, 3.0f * sin(tm));
//			renderer::SetViewPort(0, 0, P_SIZE, P_SIZE);
//			shdCB.setParameters(PL.position, PL_zNear, PL_zFar);
//			effect_shaCB.setLightParameters(PL);
//			shdCB.begin();
//			{
//				shdCB.draw(mdlMatrix, dms);
//				shdCB.draw(glm::mat4(1.0f), tnb);
//			}
//			shdCB.end();
//
//			/* ---- shadow buffer ---- */
//			renderer::SetViewPort(0, 0, gbuf.getWidth(), gbuf.getHeight());
//			shdBuffer.begin();
//			{
//				//effect_sha2D.applyEffect(shdBuffer, shd2D, gbuf, DL, lit_cam, *(gmsys.cameraPointer()));
//				effect_shaCB.applyEffect(shdBuffer, shdCB, gbuf, *(gmsys.cameraPointer()));
//			}
//			shdBuffer.end();
//
//			/* ---- soft shadow ---- */
//			renderer::SetViewPort(0, 0, gbuf.getWidth(), gbuf.getHeight());
//			shdBuffer.applyEffect(gbuf);
//
//			/* ---- post processing ---- */
//			pstp.begin();
//			pstp.process(gbuf, shdBuffer, *reinterpret_cast<cat::camera*>(gmsys.cameraPointer()));
//
//			gui::BeginFrame();
//			gui win;
//			win.Begin(" ", false, 4.0f);
//			win.Button("+-------------------+");
//			win.Button("|                   |");
//			win.Button("|      ->start      |");
//			win.Button("|      ->options    |");
//			win.Button("|      ->exit       |");
//			win.Button("|                   |");
//			win.Button("+-------------------+");
//			win.End();
//			gui::EndFrame();
//
//			pstp.end();
//		} // loop
//	}// initialize
//	gmsys.terminate();
//
//	return 0;
//}
