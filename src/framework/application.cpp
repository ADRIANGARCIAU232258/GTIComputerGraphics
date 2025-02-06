#include <cstdlib>
#include <ctime>
#include <random>
#include "application.h"
#include "entity.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 


// Creamos un vector de entidades global con varias entidades
std::vector<Entity*> entities;

// Variables para controlare la cámara
float camera_near = 0.01f;
float camera_far = 100.0f;
float camera_fov = 45.0f; 
int current_property = 0; // 0: nada, 1: near, 2: far, 3: fov

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);

	borderWidth = 1;		// Inicializamos el grosor del borde a 1
	currentMode = 0;        // Inicializamos currentMode a 0
}

Application::~Application()
{
	// Liberamos memoria de cada una de las entidades
    for (Entity* entity : entities) {
        delete entity;
    }
    entities.clear();
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;

	// Creamos unas cuantas entidades y las añadimos al vector de entidades
	Mesh* mesh1 = new Mesh();
	mesh1->LoadOBJ("../res/meshes/cleo.obj");
	Entity* entity1 = new Entity(mesh1, Matrix44());

	Mesh* mesh2 = new Mesh();
	mesh2->LoadOBJ("../res/meshes/cleo.obj");
	Entity* entity2 = new Entity(mesh2, Matrix44());

	Mesh* mesh3 = new Mesh();
	mesh3->LoadOBJ("../res/meshes/lee.obj");
	Entity* entity3 = new Entity(mesh3, Matrix44());

	entities.push_back(entity1);
	entities.push_back(entity2);
	entities.push_back(entity3);
}

// Render one frame
void Application::Render(void)
{
	framebuffer.Fill(Color::BLACK);

	// Renderizar las entidades según el modo actual
	if (currentMode == 1) {
		// Dibuja una sola entidad
		if (!entities.empty()) {
			entities[0]->Render(&framebuffer, &camera, Color::WHITE);
		}
		else {
			std::cout << "No hay entidades para renderizar" << std::endl;
		}
	}
	else if (currentMode == 2) {
		// Dibuja múltiples entidades animadas
		for (Entity* entity : entities) {
			entity->Render(&framebuffer, &camera, Color::WHITE);
		}
	}

	framebuffer.Render(); // Finalmente se va renderizando la imagen
}

// Called after render
void Application::Update(float dt)
{
	// Controlamos las propiedades de la cámara
	if (current_property == 1) {
		camera_near += 0.1f * dt;
	}
	else if (current_property == 2) {
		camera_far += 0.1f * dt;
	}
	else if (current_property == 3) {
		camera_fov += 0.1f * dt;
	}
	else if (currentMode == 2) {
		for (Entity* entity : entities) {
			entity->Update(dt);        // Actualizamos las entidades
		}
	}

	// Actualizar las matrices de la cámara
	camera.UpdateProjectionMatrix();
	camera.UpdateViewMatrix();
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app

		case SDLK_1:
		case SDLK_KP_1:
			currentMode = 1; // Dibujamos una sola entidad
			break;
		case SDLK_2:
		case SDLK_KP_2:
			currentMode = 2; // Dibujamos varias entidades animadas
			break;
		case SDLK_n:
			current_property = 1; // Configuramos propiedad actual a near
			break;
		case SDLK_f:
			current_property = 2; // Configuramos propiedad actual a far
			break;
		case SDLK_v:
			current_property = 3; // Configuramos propiedad actual a fov
			break;
		case SDLK_PLUS:
		case SDLK_KP_PLUS:
			// Incrementamos la propiedad actual
			if (current_property == 1) camera_near += 0.1f;
			else if (current_property == 2) camera_far += 0.1f;
			else if (current_property == 3) camera_fov += 1.0f;
			break;
		case SDLK_MINUS:
		case SDLK_KP_MINUS:
			// Disminuimos la propiedad actual
			if (current_property == 1) camera_near -= 0.1f;
			else if (current_property == 2) camera_far -= 0.1f;
			else if (current_property == 3) camera_fov -= 1.0f;
			break;
	}
}

void Application::OnMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;

	// ...
}

void Application::OnFileChanged(const char* filename)
{ 
	Shader::ReloadSingleShader(filename);
}