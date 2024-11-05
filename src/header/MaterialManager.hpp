#pragma once
#include "Material.hpp"

class MaterialManager {
	public:
		MaterialManager() = default;
		~MaterialManager() = default;

		void addMaterial(std::shared_ptr<Material> material) {
			APP_LOG_INFO("Added material: {0}", material->getName());
			materials.push_back(material);
		}

		void removeMaterial(int index) {
			materials.erase(materials.begin() + index);
		}

		void clearMaterials() {
			materials.clear();
		}

		std::shared_ptr<Material> getMaterial(int index) const {
			return materials[index];
		}

		int size() const {
			return materials.size();
		}

		int getIndex(std::shared_ptr<Material> material) {
			for (int i = 0; i < materials.size(); i++) {
				if (materials[i] == material) {
					return i;
				}
			}

			return -1;
		}
		
	private:
		std::vector<std::shared_ptr<Material>> materials;
};