#pragma once
#include "Scene.hpp"
#include <vector>
#include <memory>

class SceneManager {

	public:
		SceneManager();

		void addScene(Scene& scene);
		void removeScene(int index);

		Scene* getScene(int index);

	private:
		std::vector<std::shared_ptr<Scene>> scenes;
};