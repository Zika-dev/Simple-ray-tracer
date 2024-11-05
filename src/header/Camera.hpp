#pragma once
#include "Hittable.hpp"
#include "utils.hpp"
#include "SDL2/SDL.h"
#include "hittableManager.hpp"
#include <mutex>

using namespace Utils;

class Camera {
public:
	// Camera settings
	vec2 aspectRatioComponents = vec2(16.0, 9.0); // Aspect ratio of the camera
	int width = 400;
	double FOV = 90.0; // Field of view in degrees
	Point3 cameraLookFrom = Point3(0, 0, 0); // Where the camera is looking from
	Point3 cameraLookAt = Point3(0, 0, -1);  // Where the camera is looking at
	vec3 cameraUp = vec3(0, 1, 0); // Basis vector for the camera up direction

	// Rendering settings
	int maxSamples = 10;
	int maxDepth = 10;
	bool SSAA = true; // Supersampling antialiasing
	bool shading = true; // Shading
	bool gammaCorrection = true; // Gamma correction
	double gamma = 2.2;
	bool adaptiveSampling = true;
	double varianceThreshold = 0.1;
	int adaptiveSamplingDepth = 5; // Initial depth for adaptive sampling
	bool denoise = true;

	Camera(SDL_Renderer* renderer);

	std::vector<Color> render(const HittableManager& world, HDRI* hdri, bool preview = false);

	void initialize();

	vec2i getRenderSize() const {
		return renderSize;
	}

	SDL_Renderer* getRenderer() const {
		return renderer;
	}

	void setRenderWidth(int size) {
		width = size;
		initialize();
	}

	bool isRendering() const {
		return rendering;
	}

	int getTracedRays() const {
		return tracedRays;
	}

	SDL_Texture* createRenderTexture();

	void updateAspectRatio(vec2 components) {
		aspectRatioComponents = components;
		initialize();
	}

	std::mutex& getRenderPreviewMutex() {
		return renderPreviewMutex;
	}

	std::vector<Color> getPreviewBuffer() {
		return previewBuffer;
	}

private:

	int height;
	double pixelSamplesScale;
	vec2i renderSize;
	Point3 center;
	vec3 pixelDeltaU;
	vec3 pixelDeltaV;
	Point3 pixelZero;
	vec3 u, v, w; // Camera basis vectors orthonormal to each other
	bool rendering = false;
	int tracedRays = 0;
	std::mutex renderPreviewMutex;
	std::vector<Color> previewBuffer;

	SDL_Texture* renderTexture = nullptr;

	Ray getRay(vec2i pPosition);

	vec3 randomSquare();

	SDL_Renderer* renderer;

	Color rayColor(const Ray& ray, int depth, const HittableManager& world, HDRI* hdri) const ;
};