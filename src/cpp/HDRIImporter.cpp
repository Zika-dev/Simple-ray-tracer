#include "HDRIImporter.hpp"
#include "imgui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

HDRIImporter::HDRIImporter(Scene& scene, std::mutex& renderMutex) : scene(scene), renderMutex(renderMutex) {
	hdri = nullptr;
}

HDRI* HDRIImporter::loadHDRI(std::string path) {

	// Destroy previous HDRI
	if (hdri != nullptr) {
		stbi_image_free(hdri->data);
		delete hdri;
	}
	hdri = new HDRI();

	APP_LOG_INFO("Loading HDR image: {0}", path);

	// Open HDR image
	int width, height, channels;
	float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
	if (!data) {
		APP_LOG_CRITICAL("Failed to load HDR image!");

		return nullptr;
	}
	else {
		APP_LOG_INFO("HDR image loaded: {0}x{1}", width, height);
		hdri->width = width;
		hdri->height = height;
		hdri->data = data;
		hdri->name = "night8k";
	}

	return hdri;
}

void HDRIImporter::draw() {
	ImGui::Begin("HDRI Importer");
	ImGui::Text("Import an HDRI to use as a background");
	ImGui::Separator();
	ImGui::Text("Current HDRI: %s", scene.getHdri() == nullptr ? "None" : scene.getHdri()->name.c_str());
	ImGui::Separator();
	if (ImGui::Button("Import HDRI")) {
		std::string path = openFile("HDR Image (*.hdr)\0*.hdr\0");
		if (path != "") {
			std::lock_guard<std::mutex> lock(renderMutex);
			scene.setHdri(loadHDRI(path));
		}
	}
	ImGui::End();
}