#include "game/gmSys.h"
#include "glm/gtc/matrix_transform.hpp"

static const unsigned int action_release = 0;
static const unsigned int action_press   = 1;
static const unsigned int action_repeat  = 2;

extern ogm::gmSys     g_gmsys;
extern ogm::gmSurface g_gmsuf;


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

static bool g_lock_cursor = false;

static float g_dx = 0;
static float g_dy = 0;
static float g_dz = 0;
static float g_dt = 0;
static float g_dp = 0;


static void delta(float dx, float dy, float dz, float dt, float dp)
{
	glm::vec3 at  = g_gmsys.get_at();
	glm::vec3 eye = g_gmsys.get_eye();
	glm::vec3 dir = at - eye;
	if (dx || dz) {
		glm::vec3 zdir(dir.x, 0, dir.z);
		glm::vec3 xdir(-dir.z, 0, dir.x);
		glm::vec3 xyz = dz * glm::normalize(zdir) + dx * glm::normalize(xdir);
		at += xyz;
		eye += xyz;
	}
	if (dy) {
		at.y  += dy;
		eye.y += dy;
	}
	if (dp || dt) {
		glm::vec4 fnl;
		fnl = glm::rotate(glm::mat4(1.0f), dp, glm::normalize(glm::vec3(-dir.z, 0, dir.x))) * glm::vec4(dir, 0);
		fnl = glm::rotate(glm::mat4(1.0f), dt, glm::normalize(glm::vec3(0, 1, 0))) * fnl;
		glm::vec3 fnl3(fnl.x, fnl.y, fnl.z);
		float dt = dot(fnl3, glm::vec3(0, 1, 0));
		if (dt * dt <= dot(fnl3, fnl3) * 0.9f)
			at = glm::vec3(fnl.x, fnl.y, fnl.z) + eye;
		//eye = at - glm::vec3(fnl.x, fnl.y, fnl.z);
	}
	g_gmsys.set_at(at);
	g_gmsys.set_eye(eye);
}

void ctrl_get_deltas(float& dx, float& dy, float& dz, float& dt, float& dp)
{
	dx = g_dx;
	dy = g_dy;
	dz = g_dz;
	dt = g_dt;
	dp = g_dp;
}

void ctrl_handle_events()
{
	g_dx = (g_ba ? -g_cam_percisionX : 0) + (g_bd ? g_cam_percisionX : 0);
	g_dy = (g_bz ? -g_cam_percisionY : 0) + (g_bc ? g_cam_percisionY : 0);
	g_dz = (g_bs ? -g_cam_percisionZ : 0) + (g_bw ? g_cam_percisionZ : 0);
	delta(g_dx, g_dy, g_dz, 0, 0);
}

void ctrl_key(int key, int scancode, int action, int modes)
{
	if (action == action_repeat)
		return;
	if (key == '`' && action == action_press) {
		g_lock_cursor = !g_lock_cursor;
		g_gmsys.lockCursor(g_lock_cursor);
	}
	{ if (key == 'A' && action == action_press) g_ba = true; else if (key == 'A' && action == action_release) g_ba = false; }
	{ if (key == 'D' && action == action_press) g_bd = true; else if (key == 'D' && action == action_release) g_bd = false; }
	{ if (key == 'S' && action == action_press) g_bs = true; else if (key == 'S' && action == action_release) g_bs = false; }
	{ if (key == 'W' && action == action_press) g_bw = true; else if (key == 'W' && action == action_release) g_bw = false; }
	{ if (key == 'C' && action == action_press) g_bz = true; else if (key == 'C' && action == action_release) g_bz = false; }
	{ if (key == ' ' && action == action_press) g_bc = true; else if (key == ' ' && action == action_release) g_bc = false; }
	{ if (key == 256 && action == action_press) g_gmsys.exit(); }  // ESC
}

void ctrl_scroll(double xoff, double yoff)
{
	if (yoff > 0)
		delta(0, 0, g_cam_percisionZ, 0, 0);
	else if (yoff < 0)
		delta(0, 0, -g_cam_percisionZ, 0, 0);
}

void ctrl_cursor(double x, double y)
{
	if (!g_lock_cursor)
		return;
	double lastX = g_gmsys.getRealW() / 2;
	double lastY = g_gmsys.getRealH() / 2;
	g_dt = (float)(lastX - x) * g_cam_percisionT;
	g_dp = (float)(lastY - y) * g_cam_percisionP;
	
	delta(0, 0, 0, g_dt, g_dp);
}