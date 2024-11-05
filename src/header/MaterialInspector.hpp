#pragma once
#include "Scene.hpp"
#include "Material.hpp"
#include "HittableManager.hpp"

class MaterialInspector {
	public:
		MaterialInspector(Scene& scene, std::mutex& renderMutex);

		void draw();

	private:
		Scene& scene;
		Material previewMaterial;
		std::mutex& renderMutex;

		// Material properties
		float color[3];
		float fuzziness;
		float refractionIndex;
		std::string name = "Unnamed material";

		HittableManager& hittableManager;
		int lastSelectedObject = -1;

		std::shared_ptr<Material> currentMaterial;

		void updateCurrentMaterial(std::shared_ptr<Material> material);
};