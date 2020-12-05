#include "gui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void cat::gui::Initialize(const char* glsl_version, GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	ImGui::StyleColorsLight();
}

void cat::gui::BeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void cat::gui::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cat::gui::Terminate()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

float cat::gui::GetFPS()
{
	return ImGui::GetIO().Framerate;
}

void cat::gui::Begin(const char* windowName, bool collapsed, float text_size) const
{
	ImGui::Begin(windowName);
	ImGui::SetWindowFontScale(text_size);
	ImGui::SetWindowCollapsed(collapsed);
}

void cat::gui::End() const
{
	ImGui::End();
}

bool cat::gui::Button(const char* label) const
{
	return ImGui::Button(label);
}

bool cat::gui::Checkbox(const char* label, bool* v) const
{
	ImGui::Checkbox(label, v);
	return *v;
}

bool cat::gui::ColorEdit3(const char* label, float col[3]) const
{
	return ImGui::ColorEdit3(label, col);
}

bool cat::gui::ColorEdit4(const char* label, float col[4]) const
{
	return ImGui::ColorEdit4(label, col);
}

bool cat::gui::ListBox(const char* label, int* current_item, const char* const items[], int items_count) const
{
	return ImGui::ListBox(label, current_item, items, items_count);
}

bool cat::gui::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, float power) const
{
	return ImGui::SliderFloat(label, v, v_min, v_max, format, power);
}

bool cat::gui::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, float power) const
{
	return ImGui::SliderFloat2(label, v, v_min, v_max, format, power);
}

bool cat::gui::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, float power) const
{
	return ImGui::SliderFloat3(label, v, v_min, v_max, format, power);
}

bool cat::gui::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, float power) const
{
	return ImGui::SliderFloat4(label, v, v_min, v_max, format, power);
}

