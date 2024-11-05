#include "Editor.hpp"
#include "imgui.h"
#include "Log.hpp"
#include "Sphere.hpp"
#include "Mouse.hpp"
#include "keyboard.hpp"

Editor::Editor(Scene& scene, SDL_Renderer* renderer, std::mutex& renderMutex, SDL_Window* window) : scene(scene), renderer(renderer), hittableManager(scene.getHittableManager()), renderMutex(renderMutex), window(window) {

	double aspectRatio = 16.0 / 9.0;

	editorWidth = 600;
	editorHeight = editorWidth / aspectRatio;

	APP_LOG_INFO("Editor width: {0}, height: {1}", editorWidth, editorHeight);

	renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, editorWidth, editorHeight);

	// Make texture grey
	SDL_SetRenderTarget(renderer, renderTexture);
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);

	updateObjects();
};

vec2i Editor::worldToScreen(vec2 worldPos) {
	return vec2i(((worldPos.x * pixelsPerUnit) + editorWidth / 2) , (worldPos.y * pixelsPerUnit) + editorWidth / 2);
}

void drawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}

void Editor::updateObjects() {
	objects.clear();
	int objectCount = hittableManager.size();

	for (int i = 0; i < objectCount; ++i) {

		Sphere* object = static_cast<Sphere*>(hittableManager.get(i).get());

		Point3 position = object->getCenter();

		if (XZAxis) {
			Circle circle = Circle(vec2(position.x, position.z), position.y, i, vec2(position.x, position.z), object->getRadius());
			objects.push_back(circle);
		}
		else {
			Circle circle = Circle(vec2(position.x, position.y), position.z, i, vec2(position.x, -position.y), object->getRadius());
			objects.push_back(circle);
		}
		

		APP_LOG_INFO("Added circle with position: {0}, {1}, {2} ID: {2}", position.x, position.y, position.z, i);
	}
}

bool isCursorInRect(const SDL_Rect& rect, const vec2i& cursorPos) {
	return cursorPos.x >= rect.x && cursorPos.x <= rect.x + rect.w && cursorPos.y >= rect.y && cursorPos.y <= rect.y + rect.h;
}

void Editor::draw() {
	ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImGui::Text("Editor");

	if(scene.getRenderCamera().isRendering()) {
		ImGui::Text("Rendering...");
		ImGui::End();
		return;
	}

	vec2i mousePos = vec2i(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	vec2i windowPos = vec2i(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	vec2i relativePos = { ((mousePos.x - windowPos.x - 8)),
						 ((mousePos.y - windowPos.y - 65)) };
	
	bool cursorInWindow = isCursorInRect({ 0, 0, editorWidth, editorHeight }, relativePos);

	vec2 worldPos = vec2(relativePos.x, relativePos.y) / pixelsPerUnit;

	int selectedObject = scene.getSelectedObject();

	if (ImGui::IsWindowFocused() && cursorInWindow) {
		pixelsPerUnit += ImGui::GetIO().MouseWheel * (pixelsPerUnit / 10);

		if(pixelsPerUnit < 1)
			pixelsPerUnit = 1;

		if(pixelsPerUnit > 1000)
			pixelsPerUnit = 1000;

		// Movement
		if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) { // Camera panning
			for (Object& object : objects) {
				object.screenPos = object.screenPos + mouse.getDelta() / pixelsPerUnit;
			}
		}
		else if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && selectedObject != -1) { // Object dragging
			objects[selectedObject].pos.x = objects[selectedObject].pos.x + mouse.getDelta().x / pixelsPerUnit;

			if (XZAxis)
				objects[selectedObject].pos.y = objects[selectedObject].pos.y + mouse.getDelta().y / pixelsPerUnit;
			else
				objects[selectedObject].pos.y = objects[selectedObject].pos.y - mouse.getDelta().y / pixelsPerUnit; // Inverted Y axis

			objects[selectedObject].screenPos = objects[selectedObject].screenPos + mouse.getDelta() / pixelsPerUnit; // Move on screen

			// Critical section for updating object information in world
			if (renderMutex.try_lock()) {
				try {
					Sphere* object = static_cast<Sphere*>(hittableManager.get(selectedObject).get());

					if (keyboard.isPressed(SDL_SCANCODE_S)) { // Scale
						object->changeRadius(object->getRadius() + (mouse.getDelta().y + mouse.getDelta().x) / pixelsPerUnit);
						objects[selectedObject].radius = object->getRadius();
					}
					else {
						if (XZAxis)
							object->changePosition(vec3(objects[selectedObject].pos.x, objects[selectedObject].y, objects[selectedObject].pos.y)); // Move in world
						else
							object->changePosition(vec3(objects[selectedObject].pos.x, objects[selectedObject].pos.y, objects[selectedObject].y)); // Move in world
					}
				}
				catch (...) {
					renderMutex.unlock(); // Ensure unlocking in case of an exception
					throw;
				}

				renderMutex.unlock();
			}
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) { // Open object dropdown
			ImGui::OpenPopup("object_dropdown");
		}
	}

	if (ImGui::IsWindowFocused() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && cursorInWindow) { // Object selection
		int highestY = -999;

		std::vector<Circle> clickedObjects;

		for (int i = 0; i < objects.size(); ++i) { // Find highest object
			Circle object = objects[i];

			vec2i position = worldToScreen(object.screenPos);
			double radius = object.radius * pixelsPerUnit;

			SDL_Rect rect = { position.x - radius, position.y - radius, radius * 2, radius * 2 };

			if (isCursorInRect(rect, relativePos)) { // If cursor is in object
				if (object.y > highestY) {
					highestY = object.y;
					clickedObjects.push_back(object);
				}
			}
		}

		if (clickedObjects.size() > 0) {
			scene.setSelectedObject(clickedObjects[clickedObjects.size() - 1].id);
		}
		else
		{
			scene.setSelectedObject(-1);
		}
	}

	if (ImGui::Button("Switch axis")) {
		XZAxis = !XZAxis;
		updateObjects();
	}

	if (ImGui::BeginPopup("object_dropdown")) {
		if (selectedObject != -1) {
			if (ImGui::Selectable("Delete")) {
				std::lock_guard<std::mutex> lock(renderMutex);

				hittableManager.remove(selectedObject);
				updateObjects();
				scene.setSelectedObject(-1);
			}

			if (ImGui::Selectable("Deselect")) {
				scene.setSelectedObject(-1);
			}
		}

		if (selectedObject == -1)
		{
			if (ImGui::Selectable("Add Sphere")) {
				std::lock_guard<std::mutex> lock(renderMutex);

				hittableManager.add(std::make_shared<Sphere>(Point3(0, 0, 0), 0.5, scene.getMaterialManager().getMaterial(0)));
				updateObjects();
			}
		}

		ImGui::EndPopup();
	}

	if (ImGui::IsWindowFocused()) {
		SDL_SetRenderTarget(renderer, renderTexture);
		SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
		SDL_RenderClear(renderer);

		// Draw objects
		for (const Circle& object : objects) {
			// Draw red outline if selected
			if (object.id == selectedObject) {
				vec2i position = worldToScreen(object.screenPos);

				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				drawCircle(renderer, position.x, position.y, object.radius * pixelsPerUnit);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			else
			{
				vec2i position = worldToScreen(object.screenPos);
				double radius = object.radius * pixelsPerUnit;

				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				drawCircle(renderer, position.x, position.y, radius);
			}
		}

		SDL_SetRenderTarget(renderer, NULL);
	}

	ImGui::Image(renderTexture, ImVec2(editorWidth, editorHeight));

	ImGui::End();
}