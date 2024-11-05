#include "MaterialInspector.hpp"
#include "imgui.h"
#include "Sphere.hpp"
#include "Material.hpp"
#include "imgui_stdlib.h"

MaterialInspector::MaterialInspector(Scene& scene, std::mutex& renderMutex) : scene(scene), renderMutex(renderMutex), hittableManager(scene.getHittableManager()) {

	MaterialManager& materialManager = scene.getMaterialManager();

	currentMaterial = materialManager.getMaterial(0);

	Color albedo = currentMaterial->getAlbedo();
	color[0] = albedo.x;
	color[1] = albedo.y;
	color[2] = albedo.z;

	fuzziness = currentMaterial->getFuzz();
	refractionIndex = currentMaterial->getRefractionIndex();

	name = currentMaterial->getName();
};

static int currentItem = 0;

void MaterialInspector::updateCurrentMaterial(std::shared_ptr<Material> material) {
	// Update values
	currentMaterial = material;

	Color albedo = currentMaterial->getAlbedo();
	color[0] = albedo.x;
	color[1] = albedo.y;
	color[2] = albedo.z;

	fuzziness = currentMaterial->getFuzz();
	refractionIndex = currentMaterial->getRefractionIndex();

	name = currentMaterial->getName();
}

void MaterialInspector::draw() {
	ImGui::Begin("Material Inspector");

	ImGui::Text("Material Inspector");

	MaterialManager& materialManager = scene.getMaterialManager();

	std::shared_ptr<Hittable> object;

	if (scene.getSelectedObject() != -1) {
		object = hittableManager.get(scene.getSelectedObject());
		if (scene.getSelectedObject() != lastSelectedObject) {
			lastSelectedObject = scene.getSelectedObject();

			updateCurrentMaterial(object->getMaterial());

			currentItem = scene.getMaterialManager().getIndex(object->getMaterial());
		}
	}
	else {
		ImGui::Text("No object selected");
	}

	// Show all materials in a combo box
	if (ImGui::BeginCombo("Materials", materialManager.getMaterial(currentItem)->getName().c_str())) {
		for (int i = 0; i < materialManager.size(); i++) {
			bool isSelected = (currentItem == i);
			if (ImGui::Selectable(materialManager.getMaterial(i)->getName().c_str(), isSelected)) {
				currentItem = i;
				APP_LOG_INFO("Selected material: {0}", materialManager.getMaterial(i)->getName());
				
				// Update values
				updateCurrentMaterial(materialManager.getMaterial(i));
			}
			
			if(isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if (ImGui::Button("+")) {
		// Open popup to add new material
		ImGui::OpenPopup("add_material");
	}

	ImGui::SameLine();

	if (ImGui::Button("-")) {
		// See if any of the objects are using this material

		HittableManager& hittableManager = scene.getHittableManager();

		bool materialInUse = false;

		// For every object
		for (int i = 0; i < hittableManager.size(); i++) {
			std::shared_ptr<Hittable> object = hittableManager.get(i);

			if (object->getMaterial() == materialManager.getMaterial(currentItem)) {
				materialInUse = true;
				break;
			}
		}
		
		// If not, remove it
		if (!materialInUse) {
			materialManager.removeMaterial(currentItem);
			currentItem = 0;

			APP_LOG_INFO("Material removed");
		}			
		else {
			APP_LOG_WARN("Material is in use by an object");
		}
	}

	if (ImGui::BeginPopup("add_material")) {

		if (ImGui::Selectable("Lambertian")) {

			std::shared_ptr<Material> material = std::make_shared<Lambertian>("Unnamed lambertian material", Color(0.0f, 0.0f, 0.0f));

			materialManager.addMaterial(material);
		}

		if (ImGui::Selectable("Specular")) {

			std::shared_ptr<Material> material = std::make_shared<Specular>("Unnamed specular material",Color(0.0f, 0.0f, 0.0f), 0.0f);

			materialManager.addMaterial(material);
		}

		if (ImGui::Selectable("Refractive")) {

			std::shared_ptr<Material> material = std::make_shared<Refractive>("Unnamed refractive material", 0.0f);

			materialManager.addMaterial(material);
		}

		ImGui::EndPopup();
	}

	ImGui::SeparatorText("Material properties");

	// Show material properties

	MaterialType type = currentMaterial->getType();

	ImGui::InputText("Name", &name);

	if (ImGui::IsItemDeactivatedAfterEdit()) {
		std::lock_guard<std::mutex> lock(renderMutex);
		currentMaterial->setName(name);
	}

	switch (type)
	{
	case MaterialType::LAMBERTIAN:
		ImGui::Text("Type: Lambertian");
		if (ImGui::ColorEdit3("Color", color)) {
			std::lock_guard<std::mutex> lock(renderMutex);
			currentMaterial->changeAlbedo(Color(color[0], color[1], color[2]));
		}

		break;

	case MaterialType::SPECULAR:
		ImGui::Text("Type: Specular");
		if (ImGui::ColorEdit3("Color", color)) {
			std::lock_guard<std::mutex> lock(renderMutex);
			currentMaterial->changeAlbedo(Color(color[0], color[1], color[2]));
		}

		if (ImGui::SliderFloat("Fuzziness", &fuzziness, 0.0f, 1.0f)) {
			currentMaterial->changeFuzz(fuzziness);
		}

		break;

	case MaterialType::REFRACTIVE:
		ImGui::Text("Type: Refractive");
		if (ImGui::SliderFloat("Refraction index", &refractionIndex, 0.0f, 10.0f)) {
			std::lock_guard<std::mutex> lock(renderMutex);
			currentMaterial->changeRefractionIndex(refractionIndex);
		}

		break;

	case MaterialType::UNKNOWN:

		ImGui::Text("Unknown material type");

		break;

	default:

		break;
	}

	ImGui::SeparatorText("Change object");

	// Option to change material on selected object
	ImGui::BeginDisabled(scene.getSelectedObject() == -1);

	if (ImGui::Button("Apply material on selected object")) {
		std::lock_guard<std::mutex> lock(renderMutex);

		object->setMaterial(materialManager.getMaterial(currentItem));
	}

	ImGui::EndDisabled();

	ImGui::End();
}