#pragma once
#include "Scene.hpp"
#include <mutex>

class ViewportSettings {

	public:
		ViewportSettings(Scene& scene, std::mutex& renderMutex);

		void draw();

	private:
		Scene& scene;

		vec2i renderSize;

		vec2 aspectRatio;

		std::mutex& renderMutex;

		float aspectRatioX;
		float aspectRatioY;
		int renderWidth;
		int renderHeight;
		bool shading;
		int maxBounces;
		bool SSAA;
		int maxSamples;
		bool adaptiveSampling;
		int adaptiveSamplingDepth;
		double varianceThreshold;
		bool gammaCorrection;
		double gamma;
		bool denoise;
};