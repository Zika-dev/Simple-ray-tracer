#pragma once

#include <SDL2/SDL.h>
#include "utils.hpp"
#include "Scene.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include "Interval.hpp"
#include <vector>
#include "Denoiser.hpp"

using namespace Utils;

class Viewport {
public:
	Viewport(Scene& scene, Denoiser& denoiser);
	~Viewport();

	void draw();

	std::mutex& getRenderMutex() { return renderMutex; }

	float getRenderTime() { return renderTime; }

	float getElapsedTime() { return elapsedTime; }

	void requestRenderStop() {
		renderImage = false;
	}

private:
	Scene& scene;

	void viewportRender();

	std::thread renderThread;
	std::atomic<bool> running = true;
	std::mutex renderMutex;

	std::vector<Color> colorBuffer;

	bool textureReady = false;

	SDL_Texture* latestTexture = nullptr;

	float pitch, yaw;
	float sensitivity = 0.001f;
	bool skip = true; // Skip the first mouse movement since it snaps to the center of the window
	bool enableMovement = false;

	bool renderImage = false; // Render the image, not the viewport

	double scrollBuffer = 0;

	vec2 mouseDeltaBuffer = vec2(0, 0);
	vec3 lookFromBuffer = vec3(0, 0, 0);
	vec3 lookAtBuffer = vec3(0, 0, 0);

	// Delta time
	Uint64 now = SDL_GetPerformanceCounter();
	Uint64 last = 0;
	double deltaTime = 0;
	float FPS = 0;

	int FPSHistoryIndex = 0;
	float avgFPS = 0;
	Interval fpsInterval;

	int FPSHistory[10];
	int FPSHistoryPreview = 0;
	int FPSHistoryLength = 10;
	Uint64 nowPreview = SDL_GetPerformanceCounter();
	Uint64 lastPreview = 0;
	double previewDeltaTime = 0;

	int renderStart, renderEnd; // Time taken to render the image
	float renderTime = 0;
	float elapsedTime = 0;

	Denoiser& denoiser;
};