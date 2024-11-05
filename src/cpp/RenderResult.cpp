#include "RenderResult.hpp"
#include "imgui.h"
#include "utils.hpp"
#include "SDL2_image-2.8.2/include/SDL_image.h"

using namespace Utils;

RenderResult::RenderResult(Scene& scene, std::mutex& renderPreviewMutex, Denoiser& denoiser) : scene(scene), renderPreviewMutex(renderPreviewMutex), denoiser(denoiser) {
	renderTexture = nullptr;
}

void RenderResult::draw() {

	ImGui::Begin("Render Result", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::BeginDisabled(scene.getColorBuffer().empty());
			if (ImGui::MenuItem("Save Image")) {
				SDL_Surface* surface = nullptr;

				surface = ConvertTextureToSurface(renderTexture, scene.getRenderCamera().getRenderer());

				std::string filePath = saveFile("Png File (*.png)\0*.png\0");

				bool fileExists = !filePath.empty();
				if (!fileExists) {
					APP_LOG_INFO("No file selected");
					SDL_FreeSurface(surface);
				}
				else
				{
					filePath = filePath + ".png";

					IMG_SavePNG(surface, filePath.c_str());

					SDL_FreeSurface(surface);

					APP_LOG_INFO("Image saved");
				}
			}
			ImGui::EndDisabled();
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (scene.getRenderCamera().isRendering()) {
		isRendering = true;
		ImGui::Text("Rendering...");
	}

	if (isRendering) {

		{
			std::lock_guard<std::mutex> lock(renderPreviewMutex);

			previewTextureData = scene.getRenderCamera().getPreviewBuffer();
		}

		if (!previewTextureData.empty()) {
			int width = scene.getRenderCamera().getRenderSize().x;
			int height = scene.getRenderCamera().getRenderSize().y;

			if (previewTexture != nullptr) SDL_DestroyTexture(previewTexture);

			previewTexture = SDL_CreateTexture(scene.getViewCamera().getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

			createTextureFromColors(previewTexture, previewTextureData, vec2i(width, height), scene.getRenderCamera().gammaCorrection, scene.getRenderCamera().gamma);

			ImGui::Image(previewTexture, ImVec2(width, height));
		}

		if (!scene.getRenderCamera().isRendering()) { // Rendering finished
			isRendering = false;
			updateTexture = true;
		}
	}

	if (updateTexture) {
		bool dataExists = !scene.getColorBuffer().empty();
		if (dataExists) {

			APP_LOG_INFO("Updating render result");

			if (renderTexture != nullptr) SDL_DestroyTexture(renderTexture);

			width = scene.getRenderCamera().getRenderSize().x;
			height = scene.getRenderCamera().getRenderSize().y;

			renderTexture = SDL_CreateTexture(scene.getViewCamera().getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

			renderTextureData = scene.getColorBuffer();

			Camera& renderCamera = scene.getRenderCamera();

			if (renderCamera.denoise) {
				denoiser.denoise(renderTexture, renderTextureData, renderCamera.getRenderSize().x, renderCamera.getRenderSize().y, renderCamera.gammaCorrection, renderCamera.gamma);
			}
			else
			{
				createTextureFromColors(renderTexture, renderTextureData, vec2i(width, height), renderCamera.gammaCorrection, renderCamera.gamma);
			}


			updateTexture = false;
		}
	}

	if (renderTexture != nullptr && !isRendering) {
		ImGui::Image(renderTexture, ImVec2(width, height));
	}
	
	if (renderTexture == nullptr && !isRendering)
	{
		ImGui::Text("No render texture");
	}

	ImGui::End();

}