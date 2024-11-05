#include "ViewportSettings.hpp"
#include "imgui.h"


ViewportSettings::ViewportSettings(Scene& scene, std::mutex& renderMutex) : scene(scene), renderMutex(renderMutex) {
	Camera& viewCamera = scene.getViewCamera();

	renderSize = viewCamera.getRenderSize();
	aspectRatio = viewCamera.aspectRatioComponents;
	aspectRatioX = aspectRatio.x;
	aspectRatioY = aspectRatio.y;
	renderWidth = renderSize.x;
	renderHeight = renderSize.y;
	shading = viewCamera.shading;
	maxBounces = viewCamera.maxDepth;
	SSAA = viewCamera.SSAA;
	maxSamples = viewCamera.maxSamples;
	adaptiveSampling = viewCamera.adaptiveSampling;
	adaptiveSamplingDepth = viewCamera.adaptiveSamplingDepth;
	varianceThreshold = viewCamera.varianceThreshold;
	gammaCorrection = viewCamera.gammaCorrection;
	gamma = viewCamera.gamma;
};

void ViewportSettings::draw() {
	ImGui::Begin("Viewport Settings");

	ImGui::Text("Viewport Settings");

	Camera& viewCamera = scene.getViewCamera();

	ImGui::SeparatorText("Camera");

	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2 - 80);

	ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", viewCamera.cameraLookFrom.x, viewCamera.cameraLookFrom.y, viewCamera.cameraLookFrom.z);

	ImGui::SeparatorText("Resolution");

	ImGui::Text("Aspect ratio: %.3f", aspectRatio.x / aspectRatio.y);

	ImGui::InputFloat("W", &aspectRatioX);

	ImGui::SameLine();
	ImGui::InputFloat("H", &aspectRatioY);

	ImGui::InputInt("Width", &renderWidth);

	ImGui::SameLine();
	ImGui::BeginDisabled(true);
	ImGui::InputInt("Height", &renderHeight);
	ImGui::EndDisabled();

	ImGui::SeparatorText("Render settings");

	ImGui::Checkbox("Shading", &shading);
	ImGui::SameLine();
	ImGui::InputInt("Max bounces", &maxBounces);

	ImGui::NewLine();

	ImGui::Checkbox("SSAA", &SSAA);
	ImGui::SameLine();
	ImGui::BeginDisabled(!SSAA);
	ImGui::InputInt("Max samples", &maxSamples);

	ImGui::Checkbox("Adaptive sampling", &adaptiveSampling);
	ImGui::SameLine();
	ImGui::BeginDisabled(!adaptiveSampling);
	ImGui::InputInt("Adaptive sampling depth", &adaptiveSamplingDepth);
	ImGui::InputDouble("Noise threshold", &varianceThreshold);

	ImGui::EndDisabled();
	ImGui::EndDisabled();

	ImGui::NewLine();

	ImGui::Checkbox("Gamma Correction", &gammaCorrection);
	ImGui::SameLine();
	ImGui::BeginDisabled(!gammaCorrection);
	ImGui::InputDouble("Gamma", &gamma);
	ImGui::EndDisabled();

	ImGui::NewLine();
	ImGui::Checkbox("Denoise", &denoise);

	if (renderMutex.try_lock()) { // Update all settings if the render thread is not running
		try
		{
			renderHeight = viewCamera.getRenderSize().y;

			aspectRatio.x = viewCamera.aspectRatioComponents.x;
			aspectRatio.y = viewCamera.aspectRatioComponents.y;

			viewCamera.updateAspectRatio(aspectRatio);
			viewCamera.setRenderWidth(renderWidth);
			viewCamera.shading = shading;
			viewCamera.maxDepth = maxBounces;
			viewCamera.SSAA = SSAA;
			viewCamera.maxSamples = maxSamples;
			viewCamera.adaptiveSampling = adaptiveSampling;
			viewCamera.adaptiveSamplingDepth = adaptiveSamplingDepth;
			viewCamera.varianceThreshold = varianceThreshold;
			viewCamera.gammaCorrection = gammaCorrection;
			viewCamera.gamma = gamma;
			viewCamera.denoise = denoise;
		}
		catch (...)
		{
			renderMutex.unlock();
			throw;
		}
		
		renderMutex.unlock();
	}

	ImGui::End();
}