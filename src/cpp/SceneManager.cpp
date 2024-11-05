#include "SceneManager.hpp"
#include "Log.hpp"

SceneManager::SceneManager()
{

}

void SceneManager::addScene(Scene& scene)
{
	std::shared_ptr<Scene> ptr(&scene);
	scenes.push_back(ptr);
	APP_LOG_INFO("Scene added with index", scenes.size());
}

void SceneManager::removeScene(int index)
{
	scenes.erase(scenes.begin() + index);
}

Scene* SceneManager::getScene(int index)
{
	return scenes[index].get();
}