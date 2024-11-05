#include "Viewport.hpp"
#include <imgui.h>
#include "Log.hpp"
#include "utils.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "utils.hpp"

using namespace Utils;

Viewport::Viewport(Scene& scene, Denoiser& denoiser) : scene(scene), running(true), fpsInterval(0, 1000), denoiser(denoiser) {
	renderThread = std::thread(&Viewport::viewportRender, this);

	latestTexture = SDL_CreateTexture(scene.getViewCamera().getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, scene.getViewCamera().getRenderSize().x, scene.getViewCamera().getRenderSize().y);

	lookFromBuffer = scene.getViewCamera().cameraLookFrom;
	lookAtBuffer = scene.getViewCamera().cameraLookAt;

	renderTime = -1;

	pitch = 0;
	yaw = 0;
}

Viewport::~Viewport() {
	running = false;
	if (renderThread.joinable()) {
		renderThread.join();
	}
}

void Viewport::viewportRender() {
	APP_LOG_INFO("Viewport render thread started");

	while (running) {
		{
			std::lock_guard<std::mutex> lock(renderMutex);
			textureReady = false;

			if (renderImage) { // Perform main render
				APP_LOG_INFO("Rendering image...");

				renderStart = SDL_GetTicks();
				scene.render();
				renderEnd = SDL_GetTicks();
				renderTime = static_cast<float>((renderEnd - renderStart)) / 1000;

				renderImage = false;
			}
			else
			{
				// Calculate delta time
				lastPreview = nowPreview;

				colorBuffer = scene.renderView();

				nowPreview = SDL_GetPerformanceCounter();
				previewDeltaTime = (double)((nowPreview - lastPreview) * 1000 / (double)SDL_GetPerformanceFrequency());

				if (fpsInterval.contains(FPS)) { // Valid FPS
					FPSHistory[FPSHistoryIndex] = FPS;
					FPSHistoryIndex = (FPSHistoryIndex + 1) % FPSHistoryLength;
				}

				if (FPSHistoryIndex == FPSHistoryLength - 1) {
					for (int i = 0; i < FPSHistoryLength - 1; ++i) {
						avgFPS += FPSHistory[i];
					}

					avgFPS /= FPSHistoryLength - 1;
				}

				FPS = 1000 / previewDeltaTime;
			}
		}

		textureReady = true;

		std::this_thread::sleep_for(std::chrono::milliseconds(16));  // Sleep to reduce CPU usage (60 FPS)
	}

	APP_LOG_INFO("Viewport render thread ended");
}

void Viewport::draw() {
	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	Camera& viewCamera = scene.getViewCamera();

	// Calculate delta time
	last = now;
	now = SDL_GetPerformanceCounter();
	deltaTime = (double)((now - last) * 1000 / (double)SDL_GetPerformanceFrequency());

	// Display FPS
	ImGui::Text("FPS: %.2f", FPS);
	ImGui::SameLine();
	ImGui::Text("Average FPS: %.2f", avgFPS);
	ImGui::SameLine();
	ImGui::Text("Delta: %.3f", previewDeltaTime);
	ImGui::SameLine();
	ImGui::Text("FOV: %.1f", viewCamera.FOV);

	ImGui::BeginDisabled(scene.getRenderCamera().isRendering());
	if (ImGui::Button("Render")) {
		renderImage = true;
	}
	ImGui::EndDisabled();

	if (textureReady) {
		std::lock_guard<std::mutex> lock(renderMutex);

		int width, height;
		SDL_QueryTexture(latestTexture, nullptr, nullptr, &width, &height);

		vec2i renderSize = viewCamera.getRenderSize();

		if (width != renderSize.x || height != renderSize.y) {
			APP_LOG_INFO("Viewport size changed: {0} x {1} -> {2} x {3}", width, height, renderSize.x, renderSize.y);
			SDL_DestroyTexture(latestTexture);
			latestTexture = SDL_CreateTexture(viewCamera.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, renderSize.x, renderSize.y);
		}

		if (viewCamera.denoise) {
			denoiser.denoise(latestTexture, colorBuffer, width, height, viewCamera.gammaCorrection, viewCamera.gamma);
		}
		else
		{
			createTextureFromColors(latestTexture, colorBuffer, vec2i(renderSize.x, renderSize.y), viewCamera.gammaCorrection, viewCamera.gamma);
		}
	}

	if (latestTexture != nullptr) {
		ImVec2 windowSize = ImGui::GetWindowSize();
		float factor = (float)windowSize.x / viewCamera.getRenderSize().x;

		ImGui::Image(latestTexture, ImVec2(viewCamera.getRenderSize().x * factor - 20, viewCamera.getRenderSize().y * factor - 20));
	}
	else {
		ImGui::Text("No render texture");
	}

	if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered()) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			enableMovement = true;
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}
	}

	if (keyboard.isPressed(SDL_SCANCODE_ESCAPE) && enableMovement) {
		SDL_SetRelativeMouseMode(SDL_FALSE);
		enableMovement = false;
	}

	if (enableMovement) { // Buffer camera movement

		double speed = 0.001f * deltaTime; // Speed based on delta time

		vec3 forward = unitVector(viewCamera.cameraLookAt - viewCamera.cameraLookFrom); // Forward vector
		vec3 right = unitVector(crossProduct(forward, vec3(0, 1, 0))); // Right vector

		scrollBuffer += ImGui::GetIO().MouseWheel / deltaTime * 50;
		mouseDeltaBuffer = mouseDeltaBuffer + mouse.getDelta() / deltaTime;

		if (keyboard.isPressed(SDL_SCANCODE_LSHIFT)) {
			speed *= 2.0f; // Double speed
		}

		if (keyboard.isPressed(SDL_SCANCODE_W)) {
			lookFromBuffer += forward * speed / deltaTime;
			lookAtBuffer += forward * speed / deltaTime;;
		}

		if (keyboard.isPressed(SDL_SCANCODE_S)) {
			lookFromBuffer -= forward * speed / deltaTime;;
			lookAtBuffer -= forward * speed / deltaTime;;
		}

		if (keyboard.isPressed(SDL_SCANCODE_A)) {
			lookFromBuffer -= right * speed / deltaTime;;
			lookAtBuffer -= right * speed / deltaTime;;
		}

		if (keyboard.isPressed(SDL_SCANCODE_D)) {
			lookFromBuffer += right * speed / deltaTime;;
			lookAtBuffer += right * speed / deltaTime;;
		}

		if (keyboard.isPressed(SDL_SCANCODE_E)) { // Up
			lookFromBuffer += vec3(0, 1, 0) * speed / deltaTime;;
			lookAtBuffer += vec3(0, 1, 0) * speed / deltaTime;;
		}

		if (keyboard.isPressed(SDL_SCANCODE_Q)) { // Down
			lookFromBuffer -= vec3(0, 1, 0) * speed / deltaTime;;
			lookAtBuffer -= vec3(0, 1, 0) * speed / deltaTime;;
		}
	}

	if (enableMovement && textureReady) {	
		std::lock_guard<std::mutex> lock(renderMutex);

		viewCamera.FOV /= (1 + scrollBuffer / 1000); // Zoom

		if(viewCamera.FOV > 120) viewCamera.FOV = 120; // Clamp FOV
		if(viewCamera.FOV < 1) viewCamera.FOV = 1; // Clamp FOV

		scrollBuffer = 0;

		viewCamera.cameraLookFrom = lookFromBuffer;
		viewCamera.cameraLookAt = lookAtBuffer;

		// Mouse Look (Yaw and Pitch)
		yaw += mouseDeltaBuffer.x * (sensitivity * viewCamera.FOV);
		pitch -= mouseDeltaBuffer.y * (sensitivity * viewCamera.FOV);  // Inverted Y-axis for natural camera feel

		mouseDeltaBuffer = vec2(0, 0);

		// Clamp the pitch to avoid flipping the camera
		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		// Calculate new look direction
		vec3 direction;
		direction.x = cos(degreesToRadians(pitch)) * cos(degreesToRadians(yaw));
		direction.y = sin(degreesToRadians(pitch));
		direction.z = cos(degreesToRadians(pitch)) * sin(degreesToRadians(yaw));

		// Update camera lookAt point
		viewCamera.cameraLookAt = viewCamera.cameraLookFrom + direction;
	}

	if (renderImage) {
		elapsedTime = (static_cast<float>(SDL_GetTicks()) - renderStart) / 1000;
	}

	ImGui::End();
}