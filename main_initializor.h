#pragma once
#include "main_include.h"
#include "main_controller.h"

void main_initialize()
{
	renderer::SetFPS(60);
	renderer::KeyCallback(ctrl_key);
	renderer::ScrollCallback(ctrl_scroll);
	renderer::CursorCallback(ctrl_cursor);
	g_cam.perspective(0.1f, 1000.0f, 0.80f, (float)renderer::GetOriginalW() / (float)renderer::GetOriginalH());
	g_cam.walk(glm::vec3(0, 2, 5));
	g_cam.look(glm::vec3(0, 2, 0));


	unsigned sup = frameBuffer::sys_getMaxBufferCount();
	unsigned req = frameBuffer::sys_getDefinedBufferCount();
	std::cout << "max MRT supported: " << sup << std::endl;;
	std::cout << "max MRT required:  " << req << std::endl;
	if (sup < req) {
		std::cout << "error: MRT not supported!" << std::endl;
	}
}