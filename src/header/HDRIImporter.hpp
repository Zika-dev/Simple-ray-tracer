#pragma once
#include "utils.hpp"
#include "Scene.hpp"
#include <mutex>

using namespace Utils;

class HDRIImporter {
	public:
		HDRIImporter(Scene& scene, std::mutex& renderMutex);

		HDRI* loadHDRI(std::string path);

		void draw();

	private:
		Scene& scene;

		HDRI* hdri;

		std::mutex& renderMutex;
};