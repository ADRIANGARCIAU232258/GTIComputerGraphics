/*
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "entity.h"
class Application
{
public:

	// Window

	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;

	std::vector<Shader*> shaders; // Declaramos el vector de shaders
	std::vector<Shader*> texshaders; // Declaramos el vector de shaders de texturas;
	int current_shader; // Shader actual
	int current_exercise; // Ejercicio actual
	int current_lab; // Laboratorio actual

	// Input
	const Uint8* keystate;
	int mouse_state; // Tells which buttons are pressed
	Vector2 mouse_position; // Last mouse position
	Vector2 mouse_delta; // Mouse movement in the last frame

	void OnKeyPressed(SDL_KeyboardEvent event);
	void OnMouseButtonDown(SDL_MouseButtonEvent event);
	void OnMouseButtonUp(SDL_MouseButtonEvent event);
	void OnMouseMove(SDL_MouseButtonEvent event);
	void OnWheel(SDL_MouseWheelEvent event);
	void OnFileChanged(const char* filename);

	// CPU Global framebuffer
	Image framebuffer;

	Shader* quadshader;
	Shader* texshader;
	Mesh* quadmesh;
	Mesh* texmesh;
	Texture* texture;
	Texture* landscapeTexture;
	Shader* mshader;
	Texture* mtexture;
	Mesh* leemesh;
	Matrix44 model;
	Camera* camera;
	Entity* entity;
	sUniformData uniformData;
	Shader* gourshader;
	Material* gourmaterial;
	sLight light;
	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init(void);
	void Render(void);
	void Update(float dt);

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport(0, 0, width, height);
		this->window_width = width;
		this->window_height = height;
		this->framebuffer.Resize(width, height);
	}

	Vector2 GetWindowSize()
	{
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		return Vector2(float(w), float(h));
	}
};
