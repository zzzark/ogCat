#pragma once
#include <iostream>
#include "Renderer.h"
#include <chrono>
#include "common.h"
#include "camera.h"

extern cat::camera g_cam;
extern void handle_events();
extern void ctrl_key(GLFWwindow* window, int key, int scancode, int action, int mods);
extern void ctrl_scroll(GLFWwindow*, double xoff, double yoff);
extern void ctrl_cursor(GLFWwindow* window, double x, double y);
extern void getActs(float& xx, float& yy, float& zz, float& th, float& phy);