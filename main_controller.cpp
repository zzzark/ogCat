#include <iostream>
#include "Renderer.h"
#include <chrono>
#include "common.h"
#include "camera.h"

using cat::renderer;
using cat::camera;

camera g_cam;

static const float g_cam_percisionX = 0.05f;
static const float g_cam_percisionY = 0.05f;
static const float g_cam_percisionZ = 0.05f;
static const float g_cam_percisionT = 0.001f;
static const float g_cam_percisionP = 0.001f;

static bool g_ba = false;	// LEFT
static bool g_bd = false;	// RIGHT
static bool g_bw = false;	// FORWARD
static bool g_bs = false;	// BACK
static bool g_bz = false;	// DOWN
static bool g_bc = false;	// UP

static bool g_lock_cursor = true;

static float dx = 0;
static float dy = 0;
static float dz = 0;
static float dt = 0;
static float dp = 0;

void getActs(float& xx, float& yy, float& zz, float& th, float& phy)
{
	xx = dx;
	yy = dy;
	zz = dz;
	th = dt;
	phy = dp;
}
void handle_events()
{
	static auto lastTime = std::chrono::system_clock::now();
	auto nowTime = std::chrono::system_clock::now();
	double det = (double)std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - lastTime).count();
	if (det < 16)
		return;
	lastTime = nowTime;
	if (!g_lock_cursor)
		glfwSetCursorPos(renderer::GetWindow(), renderer::GetRealW() / 2, renderer::GetRealH() / 2);
	dx = (g_ba ? -g_cam_percisionX : 0) + (g_bd ? g_cam_percisionX : 0);
	dy = (g_bz ? -g_cam_percisionY : 0) + (g_bc ? g_cam_percisionY : 0);
	dz = (g_bs ? -g_cam_percisionZ : 0) + (g_bw ? g_cam_percisionZ : 0);
	if (dx || dy || dz)
		g_cam.delta(dx, dy, dz, 0, 0);
}
void ctrl_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_REPEAT)
		return;
	{ if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) g_lock_cursor = !g_lock_cursor; }
	{ if (key == GLFW_KEY_A && action == GLFW_PRESS) g_ba = true; else if (key == GLFW_KEY_A && action == GLFW_RELEASE) g_ba = false; }
	{ if (key == GLFW_KEY_D && action == GLFW_PRESS) g_bd = true; else if (key == GLFW_KEY_D && action == GLFW_RELEASE) g_bd = false; }
	{ if (key == GLFW_KEY_S && action == GLFW_PRESS) g_bs = true; else if (key == GLFW_KEY_S && action == GLFW_RELEASE) g_bs = false; }
	{ if (key == GLFW_KEY_W && action == GLFW_PRESS) g_bw = true; else if (key == GLFW_KEY_W && action == GLFW_RELEASE) g_bw = false; }
	//{ if (key == GLFW_KEY_C && action == GLFW_PRESS) g_bz = true; else if (key == GLFW_KEY_C && action == GLFW_RELEASE) g_bz = false; }
	//{ if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) g_bc = true; else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) g_bc = false; }
}
void ctrl_scroll(GLFWwindow*, double xoff, double yoff)
{
	//if (yoff > 0) g_cam.delta(0, 0, g_cam_percisionZ, 0, 0); else if (yoff < 0) g_cam.delta(0, 0, -g_cam_percisionZ, 0, 0);
}
void ctrl_cursor(GLFWwindow* window, double x, double y)
{
	if (g_lock_cursor)
		return;
	double lastX = renderer::GetRealW() / 2, lastY = renderer::GetRealH() / 2;
	dt = (float)(lastX - x) * g_cam_percisionT;
	dp = (float)(lastY - y) * g_cam_percisionP;
	glm::vec3 eye = g_cam.getEye();
	glm::vec3 at = g_cam.getAt();
	g_cam.delta(0, 0, 0, dp, dt);
	glm::vec3 det = g_cam.getAt() - g_cam.getEye();
	//_at = glm::vec3(fnl.x, fnl.y, fnl.z) + _eye;
	//_eye = _at - glm::vec3(fnl.x, fnl.y, fnl.z);
	g_cam.walk(at - det);
	g_cam.look(at);
}