#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include <fstream>
#include <iostream>

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
}

Application::~Application()
{
}

void Application::Init(void)
{


	std::ifstream vs_file("C:\\Ubicaciones JAN\\lab4_5\\res\\shaders\\quad.vs");
	std::ifstream fs_file("C:\\Ubicaciones JAN\\lab4_5\\res\\shaders\\quad.fs");

	if (!vs_file.is_open() || !fs_file.is_open()) {
		std::cerr << "Error: No se encontraron los archivos del shader." << std::endl;
	}

	std::cout << "Initiating app..." << std::endl;
	quadshader = Shader::Get("C:\\Ubicaciones JAN\\lab4_5\\res\\shaders\\quad.vs", "C:\\Ubicaciones JAN\\lab4_5\\res\\shaders\\quad.fs");
	quadmesh = new Mesh();
	quadmesh->CreateQuad();

}

// Render one frame
void Application::Render(void)
{
	glEnable(GL_DEPTH_TEST);
	quadshader->Enable();
	quadmesh->Render();
	quadshader->Disable();

	
}

// Called after render
void Application::Update(float seconds_elapsed)
{

}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch(event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
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