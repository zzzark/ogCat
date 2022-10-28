#include "../game/gmSys.h"
#include "../game/gmObject.h"
#include "../game/gmObjRenderer.h"
#include "../game/gmLight.h"
#include "../game/gmWorld.h"
#include "../game/gmParticle.h"
#include "glm/gtc/matrix_transform.hpp"
#include "main_controller.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>

using glm::vec3;
using glm::vec4;
using glm::mat4;

ogm::gmSys     g_gmsys;              // game system
ogm::gmSurface g_gmsuf;				 // game surface
ogm::gmObjRenderer g_objRenderer;	 // for rendering objects
ogm::gmPointLight g_pntLight;		 // point light
ogm::gmDirectionalLight g_dirLight;	 // directional light
ogm::gmSky g_sky1;					 // light sky
ogm::gmSky g_sky2;					 // dark sky

ogm::gmDust g_dust;                  // dust
ogm::gmFireworks g_fire;			 // fireworks

// lights color
const glm::vec3 g_dir_col(2, 2, 1.88f);  // directional light
const glm::vec3 g_pnt_col(10, 10, 14);   // point light

// intensity of the dust
const float g_dust_intensity = 2.5f;

// flags to control dark or light environments
bool g_b_switch_dark = false;
bool g_b_switch_light = false;
bool g_b_dust = false;
bool g_b_fire = false;
bool g_b_pntLight = true;
bool g_b_dirLight = true;
float r_dark = 0.0f;
float r_light = 0.0f;


// total time
unsigned long long g_time = 0;

// foods found
int g_food = 0;


struct
{
private:
	int _iterator = 0;
	int _dx = 8;
	int _dy = 8;
	int _x = 0;
	int _y = 0;
public:
	void set_range(int dx, int dy) {
		reset();
		_dx = dx; _dy = dy;
	}
	void move_range(int x, int y) {
		reset();
		_x = x; _y = y;
	}
	void reset() { _iterator = 0; }
	bool valid() { return _iterator < (2 * _dx - 1)* (2 * _dy - 1); }
	bool next_valid() {
		int x = _iterator % (2 * _dx - 1);
		int y = _iterator / (2 * _dx - 1);
		x += _x - _dx;
		y += _y - _dy;
		if (0 <= x && x < (int)objs.size() && 0 <= y && y < (int)objs[0].size()) {
			return true;
		}
		return false;
	}
	ogm::gmObj& next_obj() {
		int x = _iterator % (2 * _dx - 1);
		int y = _iterator / (2 * _dx - 1);
		x += _x - _dx;
		y += _y - _dy;
		return objs[x][y];
	}
	void next() {
		_iterator++;
	}
	void release_all() {
		{std::vector<std::vector<ogm::gmObj> > tmp; tmp.swap(objs); }
		{boxmans[0].release(); boxmans[1].release(); }
		{boxman = nullptr; }
		{ground.release(); }
		{bird.release(); }
	}

public:
	std::vector<std::vector<ogm::gmObj> > objs;
	std::vector < std::vector<int> > objs_type;
	ogm::gmObj boxmans[2];
	ogm::gmObj* boxman = &boxmans[0];
	ogm::gmGround ground;
	ogm::gmObj bird;

	glm::vec3 last_at0  = glm::vec3(1);
	glm::vec3 last_eye0 = glm::vec3(1);
	glm::vec3 last_at1  = glm::vec3(1);
	glm::vec3 last_eye1 = glm::vec3(1);
} g_objs;

void game_switch_to_dark()
{
	g_b_switch_dark = true;
	g_b_switch_light = false;
	r_dark = 0.0f;
}

void game_switch_to_light()
{
	g_b_switch_dark = false;
	g_b_switch_light = true;
	r_light = 0.0f;
}

// 
void game_initialize()
{
	g_gmsys.initialize(1200, 800, "2019151030_ZhengRuiKun_QiMoDaZuoYe");
	g_gmsys.setCurposCallback(ctrl_cursor);
	g_gmsys.setKeybrdCallback(ctrl_key);
	g_gmsys.setScrollCallback(ctrl_scroll);
	g_gmsuf.initialize();
	g_objRenderer.initialize();

	// camera
	g_gmsys.set_eye(glm::vec3(4, 5, 12));
	g_gmsys.set_at(glm::vec3(4, 0, 4));
	
	// boxman
	g_objs.last_eye0 = glm::vec3(4, 5, 12);
	g_objs.last_at0 = glm::vec3(4, 0, 4);

	g_objs.boxmans[0].createFromFile("res/objects/boxman0.txt");
	g_objs.boxmans[0].moveTo(glm::vec3(4, 0, 4));
	g_objs.boxmans[0].stop();
	g_objs.boxmans[0].setID(glm::vec3(2));

	g_objs.last_eye1 = glm::vec3(26, 5, 34);
	g_objs.last_at1 = glm::vec3(26, 0, 26);

	g_objs.boxmans[1].createFromFile("res/objects/boxman1.txt");
	g_objs.boxmans[1].moveTo(glm::vec3(26, 0, 26));
	g_objs.boxmans[1].stop();
	g_objs.boxmans[1].setID(glm::vec3(3));

	// bird
	g_objs.bird.createFromFile("res/objects/bird.txt");
	g_objs.bird.moveTo(glm::vec3(0, 4, 6));
	g_objs.bird.loop();
	g_objs.bird.setID(glm::vec3(4));

	// level info
	std::ifstream ifs("res/game/levels/level_1");
	if (!ifs) {
		std::cout << "Error: cannot open file: level_1" << std::endl;
	}
	std::string str_tmp;
	do {
		std::getline(ifs, str_tmp);
	} while (str_tmp != "#end");
	int w, h;
	ifs >> h >> w;
	int i = -1;
	g_objs.objs_type.resize((size_t)h);
	for (auto& objs : g_objs.objs_type) {
		objs.resize((size_t)w);
	}
	g_objs.objs.resize((size_t)h);
	for (auto& objs : g_objs.objs) {
		++i;
		int j = -1;
		objs.resize((size_t)w);
		for (auto& obj : objs) {
			++j;
			int val;
			ifs >> val;
			g_objs.objs_type[i][j] = val;
			if (val == 0) { continue; }
			else if (val == 1) {
				obj.createFromFile("res/objects/wall.txt");
			}
			else if (val == 2) {
				obj.createFromFile("res/objects/column.txt");
			}
			else if (val == 3) {
				obj.createFromFile("res/objects/food.txt");
			}
			else if (val == 4) {
				obj.createFromFile("res/objects/poison.txt");
			}
			else if (val == 5) {
				obj.createFromFile("res/objects/antidote.txt");
			}
			obj.setID(glm::vec3((float)((i + 2) * 1000 + (j + 2))));
			obj.move((float)(i + 1) * 2.0f, 0, (float)(j + 1) * 2.0f);
		}
	}

	// ground
	g_objs.ground.create("res/textures/ground.jpg", "res/textures/terrain.jpg",
		0.1f, 2.0f, 1.0f, 2.0f, 2.0f, 0);
	 
	// skys
	g_sky1.create("res/textures/s1/1.jpg", "res/textures/s1/2.jpg", "res/textures/s1/3.jpg",
		"res/textures/s1/4.jpg", "res/textures/s1/5.jpg", "res/textures/s1/6.jpg");
	g_sky2.create("res/textures/s2/1.jpg", "res/textures/s2/2.jpg", "res/textures/s2/3.jpg",
		"res/textures/s2/4.jpg", "res/textures/s2/5.jpg", "res/textures/s2/6.jpg");
	g_sky1.setIntensity(1.0f);
	g_sky2.setIntensity(0.0f);

	// particles
	g_dust.createDust("res/textures/dust.jpg", 2000, (float)(w * 2), (float)5, .3f);
	g_dust.move((float)w, (float)w);
	g_dust.setSize(0.05f);
	g_dust.setIntensity(g_dust_intensity);
	g_dust.setRate(0.1f);
	g_b_dust = false;

	g_fire.createFireworks("res/textures/fire.jpg", 500, glm::vec3(0), 2, 0.05f);
	g_fire.setSize(0.05f);
	g_fire.setIntensity(2.0f);
	g_fire.setRate(2.5f);
	g_fire.loop(false);
	g_b_fire = true;

	// lights
	g_dirLight.create();
	g_pntLight.create();
	g_dirLight.move(0, 0, 1, 1, 6, 0);
	g_pntLight.move(0, 6, 0);
	g_dirLight.setRGB(g_dir_col.r, g_dir_col.g, g_dir_col.b);
	g_pntLight.setRGB(g_pnt_col.r, g_pnt_col.g, g_pnt_col.b);

	g_b_pntLight = false;  // disable point light
	g_b_dirLight = true;   // enable directional light
}

void game_terminate()
{
	g_objs.release_all();

	g_dust.destroy();
	g_fire.destroy();
	
	g_sky1.release();
	g_sky2.release();

	g_pntLight.release();
	g_dirLight.release();

	g_objRenderer.terminate();
	g_gmsuf.terminate();
	g_gmsys.terminate();
}

/* ---- interaction ---- */
bool game_interact(ogm::gmObj& obj, int type, int i, int j)
{
	if (type == 3) {
		obj.release();
		g_fire.move(glm::vec3((float)(i + 1) * 2.0f, 0, (float)(j + 1) * 2.0f));
		g_fire.reset();
		g_food++;
		if (g_food == 10) {
			g_gmsys.exit();
		}
		return false;
	}
	else if (type == 4) {
		obj.release();
		g_fire.move(glm::vec3((float)(i + 1) * 2.0f, 0, (float)(j + 1) * 2.0f));
		g_fire.reset();

		game_switch_to_dark();
		return false;
	}
	else if (type == 5) {
		obj.release();
		g_fire.move(glm::vec3((float)(i + 1) * 2.0f, 0, (float)(j + 1) * 2.0f));
		g_fire.reset();

		game_switch_to_light();
		return false;
	}
	return true;
}

/* ---- handle game events ---- */
void game_events()
{
	if (g_b_switch_dark) {
		g_b_dust = true;
		g_b_pntLight = true;
		g_b_dirLight = true;

		if (g_gmsys.getTimeDelta() >= 16) {
			r_dark += 0.2f * ((float)g_gmsys.getTimeDelta() / 160.0f);
			if (r_dark > 1.0f) r_dark = 1.0f;
		}

		const float b_dark = 1.0f - r_dark;
		g_pntLight.setRGB(g_pnt_col.r * r_dark, g_pnt_col.g * r_dark, g_pnt_col.b * r_dark);
		g_dirLight.setRGB(g_dir_col.r * b_dark, g_dir_col.g * b_dark, g_dir_col.b * b_dark);
		g_dust.setIntensity(r_dark * g_dust_intensity);
		g_sky1.setIntensity(b_dark);
		g_sky2.setIntensity(r_dark);

		if (r_dark == 1.0f) {
			g_b_dirLight = false;
			g_b_switch_dark = false;
		}
	}
	if (g_b_switch_light) {
		g_b_pntLight = true;
		g_b_dirLight = true;

		if (g_gmsys.getTimeDelta() >= 16) {
			r_light += 0.2f * ((float)g_gmsys.getTimeDelta() / 160.0f);
			if (r_light > 1.0f) r_light = 1.0f;
		}

		const float b_light = 1.0f - r_light;
		g_pntLight.setRGB(g_pnt_col.r * b_light, g_pnt_col.g * b_light, g_pnt_col.b * b_light);
		g_dirLight.setRGB(g_dir_col.r * r_light, g_dir_col.g * r_light, g_dir_col.b * r_light);
		g_dust.setIntensity(b_light * g_dust_intensity);
		g_sky1.setIntensity(r_light);
		g_sky2.setIntensity(b_light);

		if (r_light == 1.0f) {
			g_b_dust = false;
			g_b_pntLight = false;
			g_b_switch_light = false;
		}
	}
}


/* ---- game control ---- */
void game_control()
{
	// V
	bool v = false;
	ctrl_get_keys(v);
	if (v) {
		if (g_objs.boxman == &g_objs.boxmans[0]) {
			g_objs.last_at0  = g_gmsys.get_at();
			g_objs.last_eye0 = g_gmsys.get_eye();
			g_gmsys.set_eye(g_objs.last_eye1);
			g_gmsys.set_at(g_objs.last_at1);
			g_objs.boxman->stop();
			g_objs.boxman = &g_objs.boxmans[1];
		}
		else {
			g_objs.last_at1 = g_gmsys.get_at();
			g_objs.last_eye1 = g_gmsys.get_eye();
			g_gmsys.set_eye(g_objs.last_eye0);
			g_gmsys.set_at(g_objs.last_at0);
			g_objs.boxman->stop();
			g_objs.boxman = &g_objs.boxmans[0]; 
		}
		g_objs.move_range((int)(g_objs.boxman->get_pos().x / 2), (int)(g_objs.boxman->get_pos().z / 2));
		return;
	}

	// A, D
	glm::vec3 delta; float theta;
	float dx, dy, dz, dt, dp;
	ctrl_handle_events();
	ctrl_get_deltas(dx, dy, dz, dt, dp);

	auto dir = glm::normalize(g_gmsys.get_at() - g_gmsys.get_eye());
	auto drz = glm::normalize(glm::vec3(dir.x, 0, dir.z));
	auto drx = glm::normalize(glm::vec3(-dir.z, 0, dir.x));

	theta = glm::dot(glm::vec3(0, 0, -1), drz);
	theta = glm::acos(theta) + 3.141f;
	if (drz.x > 0) theta = -theta;
	delta = drx * dx + drz * dz;

	if (delta.x == 0 && delta.z == 0)
		g_objs.boxman->stop();
	else
		g_objs.boxman->loop();
	glm::vec3 pos = g_objs.boxman->get_pos();
	glm::mat4 last_mat = g_objs.boxman->get_model_matrix();
	g_objs.boxman->get_model_matrix() = glm::rotate(glm::mat4(1), theta, glm::vec3(0, 1, 0));
	g_objs.boxman->moveTo(pos.x + delta.x, 0, pos.z + delta.z);
	
	for (auto& bm : g_objs.boxmans) {
		if (&bm == g_objs.boxman)
			continue;
		if (collision::basicBlock::test(bm, *g_objs.boxman)) {
			g_objs.boxman->get_model_matrix() = last_mat;
			ctrl_back();
			return;
		}
	}

	int i = -1;
	for (auto& objs : g_objs.objs) {
		++i;
		int j = -1;
		for (auto& obj : objs) {
			++j;
			if (!obj.isValid())
				continue;
			if (collision::basicBlock::test(obj, *g_objs.boxman)) {
				if (game_interact(obj, g_objs.objs_type[i][j], i, j)) {
					g_objs.boxman->get_model_matrix() = last_mat;
					ctrl_back();
					return;
				}
			}
		}
	}
	g_dirLight.move(g_objs.boxman->get_pos().x, g_objs.boxman->get_pos().z, 1, 1, 6, 0);
	g_pntLight.move(g_objs.boxman->get_pos().x, 6, g_objs.boxman->get_pos().z);
	g_objs.move_range((int)(g_objs.boxman->get_pos().x / 2), (int)(g_objs.boxman->get_pos().z / 2));
}


void game_start()
{
	std::cout << "+------------------------------+" << std::endl;
	std::cout << "|   # 操作说明:                 " << std::endl;
	std::cout << "|      ~: 锁定/解锁鼠标          " << std::endl;
	std::cout << "|      W: 向前移动              " << std::endl;
	std::cout << "|      S: 向后移动              " << std::endl;
	std::cout << "|      V: 切换角色              " << std::endl;
	std::cout << "|      鼠标: 控制方向            " << std::endl;
	std::cout << "|   # 获胜条件:                 " << std::endl;
	std::cout << "|      找到全部10个黄色方块       " << std::endl;
	std::cout << "|   # 其他:                     " << std::endl;
	std::cout << "|      暗红色方块会导致视野变小    " << std::endl;
	std::cout << "|      找到蓝色方块来解决!        " << std::endl;
	std::cout << "+------------------------------+" << std::endl;
	std::cout << "                           " << std::endl;
	std::cout << "  ##### 按任意键开始 #####  " << std::endl;
	system("pause");
}

void game_end()
{
	if (g_food != 10)
		return;
	unsigned long long delta = g_time;
	auto m = (delta / 60000);
	auto s = (delta - m * 60000) / 1000;
	std::cout << "  ## 恭喜获胜 ##  " << std::endl;
	std::cout << "共耗时: " << m << "分" << s<< "秒" << std::endl;
	std::cout << "  ##### 按任意键结束 #####  " << std::endl;
	system("pause");
}

int main()
{
	game_start();
	game_initialize();
	g_time = g_gmsys.getNowTime();

	while (!g_gmsys.shouldExitProgram())
	{
		// ---- handle evnets ---- //
		game_control();
		game_events();

		// ---- before rendering ---- //
		g_gmsys.refresh();
		g_gmsuf.prepare();

		// ---- render objects ---- //
		g_gmsuf.begin_rendering();
		
		// boxmans
		g_objRenderer.begin(g_gmsuf);
		for (auto& obj : g_objs.boxmans) {
			g_objRenderer.render(obj, g_gmsuf, g_gmsys);
		}

		// objs
		g_objs.reset();
		while (g_objs.valid()) {
			if (g_objs.next_valid()) {
				auto& obj = g_objs.next_obj();
				g_objRenderer.render(obj, g_gmsuf, g_gmsys);
			}
			g_objs.next();
		}

		// bird
		g_objRenderer.render(g_objs.bird, g_gmsuf, g_gmsys);

		// ground
		g_objs.ground.renderTheGround(g_objRenderer, g_gmsys, g_gmsuf);
		g_objRenderer.end();

		// sky
		g_sky1.renderTheSkey(g_gmsys, g_gmsuf);
		g_sky2.renderTheSkey(g_gmsys, g_gmsuf);

		// particles
		if (g_b_dust) g_dust.render(g_gmsuf, g_gmsys);
		if (g_b_fire) g_fire.render(g_gmsuf, g_gmsys);
		
		g_gmsuf.end_rendering();

		// ---- render shadows ---- //
		if (g_b_pntLight) {
			g_pntLight.begin();

			// boxmans
			for (auto& obj : g_objs.boxmans) {
				g_pntLight.draw(obj);
			}

			// bird
			g_pntLight.draw(g_objs.bird);

			// objs
			g_objs.reset();
			while (g_objs.valid()) {
				if (g_objs.next_valid()) {
					auto& obj = g_objs.next_obj();
					g_pntLight.draw(obj);
				}
				g_objs.next();
			}

			g_pntLight.end();
		}
		if (g_b_dirLight) {
			g_dirLight.begin();

			// boxmans
			for (auto& obj : g_objs.boxmans) {
				g_dirLight.draw(obj);
			}

			// bird
			g_dirLight.draw(g_objs.bird);

			// objs
			g_objs.reset();
			while (g_objs.valid()) {
				if (g_objs.next_valid()) {
					auto& obj = g_objs.next_obj();
					g_dirLight.draw(obj);
				}
				g_objs.next();
			}

			g_dirLight.end();
		}

		// ---- apply shadows ---- //
		g_gmsuf.begin_shadow();
		if (g_b_dirLight) g_dirLight.apply(g_gmsys, g_gmsuf);
		if (g_b_pntLight) g_pntLight.apply(g_gmsys, g_gmsuf);
		g_gmsuf.end_shadow();


		// ---- apply effects ---- //
		g_gmsuf.begin_process();
		g_gmsuf.process(g_gmsys);
		g_gmsuf.end_process();

	}
	game_terminate();
	g_time = g_gmsys.getNowTime() - g_time;
	game_end();
	return 0;
}