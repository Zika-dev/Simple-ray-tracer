#include <SDL2/SDL.h>
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_version.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL2_image-2.8.2/include/SDL_image.h>

#include "imgui.h"

#include "Log.hpp"

#include "Viewport.hpp"
#include "Settings.hpp"
#include "RenderSettings.hpp"
#include "RenderResult.hpp"
#include "viewportSettings.hpp"
#include "Editor.hpp"
#include "MaterialInspector.hpp"
#include "HDRIImporter.hpp"

#include "Keyboard.hpp"
#include "Mouse.hpp"

#include "utils.hpp"
#include "Renderer.hpp"

#include "HittableManager.hpp"
#include "Sphere.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Scene.hpp"
#include "SceneManager.hpp"

#include "Denoiser.hpp"

using namespace Utils;

int main(int, char**) {

	Log::init();

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* renderTexture = nullptr;

	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		APP_LOG_CRITICAL("Error: SDL_Init(): {0}\n", SDL_GetError());
		return -1;
	}
	
	// Create window with SDL_Renderer graphics context
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	window = SDL_CreateWindow("Ray tracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
	if (window == nullptr)
	{
		APP_LOG_CRITICAL("Error: SDL_CreateWindow(): {0}\n", SDL_GetError());
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		APP_LOG_CRITICAL("Error: SDL_CreateRenderer(): {0}\n", SDL_GetError());
		return -1;
	}

	SDL_Renderer* renderer2 = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	APP_LOG_WARN("SDL initialized");

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);

	// Our state
	bool show_demo_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	APP_LOG_WARN("Dear ImGui initialized");

	HittableManager hittableManager;

	MaterialManager materialManager;

	materialManager.addMaterial(std::make_shared<Lambertian>("ground", Color(0.8, 0.8, 0.0)));
	materialManager.addMaterial(std::make_shared<Lambertian>("center", Color(0.1, 0.2, 0.5)));
	materialManager.addMaterial(std::make_shared<Lambertian>("center middle", Color(0.7, 0.3, 0.3)));
	materialManager.addMaterial(std::make_shared<Lambertian>("center top", Color(0.8, 0.8, 0.8)));
	materialManager.addMaterial(std::make_shared<Specular>("left", Color(0.8, 0.8, 0.8), 0.0));
	materialManager.addMaterial(std::make_shared<Specular>("right", Color(0.8, 0.6, 0.2), 0.0));
	materialManager.addMaterial(std::make_shared<Refractive>("bubble", 1.5));

	hittableManager.add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, materialManager.getMaterial(0)));
	hittableManager.add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.0), 0.5, materialManager.getMaterial(1)));
	hittableManager.add(std::make_shared<Sphere>(Point3(0.0, 0.85, -1.0), 0.35, materialManager.getMaterial(2)));
	hittableManager.add(std::make_shared<Sphere>(Point3(0.0, 1.4, -1.0), 0.2, materialManager.getMaterial(3)));
	hittableManager.add(std::make_shared<Sphere>(Point3(-1.5, 0.0, -1.0), 0.5, materialManager.getMaterial(4)));
	hittableManager.add(std::make_shared<Sphere>(Point3(-1.5, 0.0, -1.0), 0.4, materialManager.getMaterial(5)));
	hittableManager.add(std::make_shared<Sphere>(Point3(1.5, 0.0, -1.0), 0.5, materialManager.getMaterial(6)));

	Camera camera(renderer);

	camera.aspectRatioComponents = vec2(16.0, 9.0);
	camera.width = 400;
	camera.FOV = 20;
	camera.cameraLookFrom = Point3(0, 2, 10);
	camera.cameraLookAt = Point3(0, 0.5, -1);
	camera.cameraUp = vec3(0, 1, 0);

	camera.maxSamples = 100;
	camera.maxDepth = 50;
	camera.SSAA = true;
	camera.shading = true;
	camera.gammaCorrection = true;
	camera.gamma = 2.2;
	camera.varianceThreshold = 0.1;
	camera.adaptiveSampling = true;
	camera.adaptiveSamplingDepth = 32;

	APP_LOG_INFO("Camera initialized");

	camera.initialize(); // Calculate camera parameters

	// Scene
	SceneManager sceneManager;
	Scene mainScene(camera, hittableManager, materialManager);
	sceneManager.addScene(mainScene);

	// Denoiser
	Denoiser denoiser;

	// Windows
	Viewport viewport(mainScene, denoiser);

	Settings settings;

	RenderSettings renderSettings(mainScene, renderer, viewport);

	RenderResult renderResult(mainScene, camera.getRenderPreviewMutex(), denoiser);

	ViewportSettings viewportSettings(mainScene, viewport.getRenderMutex());
	
	Editor editor(mainScene, renderer, viewport.getRenderMutex(), window);

	MaterialInspector materialInspector(mainScene, viewport.getRenderMutex());

	HDRIImporter hdriImporter(mainScene, viewport.getRenderMutex());

	bool running = true;
	while (running)
	{
		mouse.setDelta (vec2(0, 0));

		// Poll and handle events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{

			ImGui_ImplSDL2_ProcessEvent(&event);
			keyboard.update(event);

			if (event.type == SDL_QUIT)
				running = false;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				running = false;

			if (event.type == SDL_MOUSEMOTION) {
				int deltaX = event.motion.xrel;
				int deltaY = event.motion.yrel;
				mouse.setDelta(vec2(deltaX, deltaY));
			}
		}

		// Minimized window
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
		{
			SDL_Delay(100);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		viewport.draw();
		//settings.draw(100);
		renderSettings.draw();
		renderResult.draw();
		viewportSettings.draw();
		editor.draw();
		materialInspector.draw();
		hdriImporter.draw();

		// Rendering
		ImGui::Render();
		SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

		SDL_SetRenderTarget(renderer, NULL);

		SDL_RenderPresent(renderer);
	}

	// Cleanup
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}