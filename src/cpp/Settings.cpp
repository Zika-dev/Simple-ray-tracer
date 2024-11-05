#include "Settings.hpp"
#include "Log.hpp"

#include "imgui.h"

Settings::Settings() {
	ImGui::SetNextWindowFocus();
}

void Settings::draw(float avgFPS) {
	ImGui::Begin("Settings");

	ImGui::Text("Average FPS: %f", avgFPS);

	ImGui::End();
}