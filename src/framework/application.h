/*  
	+ This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "camera.h"

class Application
{
public:

	bool drawLines = false;			// Varriables para guardar el estado de la creaci�n de primitivas
	bool drawRectangles = false;
	bool drawCircles = false;
	bool drawTriangles = false;
	bool isFilled = false;

	std::vector<Entity*> entities; // Definimos vector para almacenar las entidades

	// Declaramos la c�mara
	Camera camera;
	Vector3 camera_target;
	float camera_distance;

	bool rightMouseButtonDown = false;
	bool leftMouseButtonDown = false;


	// Window

	SDL_Window* window = nullptr;
	int window_width;
	int window_height;

	float time;

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

	// Constructor and main methods
	Application(const char* caption, int width, int height);
	~Application();

	void Init( void );
	void Render( void );
	void Update( float dt );

	// Other methods to control the app
	void SetWindowSize(int width, int height) {
		glViewport( 0,0, width, height );
		this->window_width = width;
		this->window_height = height;
		this->framebuffer.Resize(width, height);
	}

	Vector2 GetWindowSize()
	{
		int w,h;
		SDL_GetWindowSize(window,&w,&h);
		return Vector2(float(w), float(h));
	}

private: 
	int borderWidth;	// Definimos una variable para incrementar o reducir el grosor del borde del rect�ngulo
	int currentMode = 0;		//Definimos currentMode para utilizarlo en la detecci�n de teclas
};
