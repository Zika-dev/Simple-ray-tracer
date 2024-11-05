#include "RenderSettings.hpp"
#include "imgui.h"

RenderSettings::RenderSettings(Scene& scene, SDL_Renderer* renderer, Viewport& viewport) : scene(scene), renderer(renderer), viewport(viewport) {

}

void RenderSettings::draw() {
	ImGui::Begin("Render Settings");

	ImGui::Text("Render Settings");

	Camera& renderCamera = scene.getRenderCamera();

	renderSize.y = renderCamera.getRenderSize().y;
	renderSize.x = renderCamera.getRenderSize().x;

	aspectRatio.x = renderCamera.aspectRatioComponents.x;
	aspectRatio.y = renderCamera.aspectRatioComponents.y;

	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2 - 80);

	ImGui::BeginDisabled(renderCamera.isRendering());

	ImGui::SeparatorText("Resolution");

	ImGui::Text("Aspect ratio: %.3f", aspectRatio.x / aspectRatio.y);

	ImGui::InputFloat("W", &aspectRatio.x);

	if (ImGui::IsItemDeactivatedAfterEdit()) {
		renderCamera.updateAspectRatio(aspectRatio);
	}

	ImGui::SameLine();
	ImGui::InputFloat("H", &aspectRatio.y);

	if (ImGui::IsItemDeactivatedAfterEdit()) {
		renderCamera.updateAspectRatio(aspectRatio);
	}

	ImGui::InputInt("Width", &renderSize.x);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		renderCamera.setRenderWidth(renderSize.x);
	}

	ImGui::SameLine();
	ImGui::BeginDisabled(true);
	ImGui::InputInt("Height", &renderSize.y);
	ImGui::EndDisabled();

	ImGui::SeparatorText("Render settings");

	ImGui::NewLine();

	ImGui::Checkbox("Shading", &renderCamera.shading);
	ImGui::SameLine();
	ImGui::InputInt("Max bounces", &renderCamera.maxDepth);

	ImGui::NewLine();

	ImGui::Checkbox("SSAA", &renderCamera.SSAA);
	ImGui::SameLine();
	ImGui::BeginDisabled(!renderCamera.SSAA); // Disable all inputs if SSAA is disabled
	ImGui::InputInt("Max samples", &renderCamera.maxSamples);
	
	ImGui::Checkbox("Adaptive sampling", &renderCamera.adaptiveSampling);
	ImGui::SameLine();
	ImGui::BeginDisabled(!renderCamera.adaptiveSampling); // Disable adaptive sampling depth if adaptive sampling is disabled
	ImGui::InputInt("Adaptive sampling depth", &renderCamera.adaptiveSamplingDepth);

	ImGui::InputDouble("Noise threshold", &renderCamera.varianceThreshold);
	ImGui::EndDisabled();
	ImGui::EndDisabled();

	ImGui::Checkbox("Gamma Correction", &renderCamera.gammaCorrection);
	ImGui::SameLine();
	ImGui::BeginDisabled(!renderCamera.gammaCorrection);
	ImGui::InputDouble("Gamma", &renderCamera.gamma);
	ImGui::EndDisabled();

	ImGui::NewLine();
	ImGui::Checkbox("Denoise", &renderCamera.denoise);

	ImGui::EndDisabled();

	if (renderCamera.isRendering()) {
		ImGui::Text("Rendering...");
		ImGui::Text("Elapsed: %.2f", viewport.getElapsedTime());
		//if (ImGui::Button("Stop render")) {
		//	viewport.requestRenderStop();
		//}
	}

	if (!renderCamera.isRendering()) {
		ImGui::Text("Time to render: %.2f", viewport.getRenderTime());
	}

	ImGui::End();
}