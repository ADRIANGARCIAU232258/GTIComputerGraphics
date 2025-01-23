#include <cstdlib>
#include <ctime>
#include <random>
#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 


ParticleSystem particleSystem;

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
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;
	particleSystem.Init();		// Con esto incializamos el sistema de creaci�n de part�culas
}

// Render one frame
void Application::Render(void)
{
	framebuffer.Fill(Color::BLACK);
	
	if (drawLines) {
		framebuffer.DrawLineDDA(300, 300, 400, 400, Color::WHITE);		// Dibujamos una l�nea
	}
	else if (drawRectangles) {
		framebuffer.DrawRect(200, 200, 100, 100, Color::RED, borderWidth, isFilled, Color::GREEN);			// Aqu� un rect�ngulo
	}
	else if (drawCircles) {
		framebuffer.DrawCircle(500, 500, 100, Color::YELLOW, borderWidth, isFilled, Color::PURPLE);			// Un c�rculo
	}
	else if (drawTriangles) {				// Un tri�ngulo
		Vector2 p0 = { 450, 200 };
		Vector2 p1 = { 600, 375 };
		Vector2 p2 = { 400, 450 };

		framebuffer.DrawTriangle(p0, p1, p2, Color::BLUE, isFilled, Color::CYAN);
	}
	else if (currentMode == 6) {
		particleSystem.Render(&framebuffer);		// Aqu� renderizamos el sistema de particulas para mostrarlas por pantalla
	}

	framebuffer.Render();		// Finalmente se va renderizando la imagen
}

// Called after render
void Application::Update(float dt)
{
	if (currentMode == 6) {
		particleSystem.Update(dt);		// Aqu� actualizamos el sistema de part�culas
	}
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app

		case SDLK_PLUS:
		case SDLK_KP_PLUS: borderWidth++; break;	// Incrementamos el grosor del borde 

		case SDLK_MINUS:
		case SDLK_KP_MINUS: borderWidth = std::max(1, borderWidth - 1); break;		// Reducimos el grosor del borde asegur�donos que sea mayor que 1

		case SDLK_KP_1:
		case SDLK_1: {				// Si pulsamos la tecla 1 tanto de normal como en el KeyPad...
			drawLines = true;
			drawRectangles = false;
			drawCircles = false;
			drawTriangles = false;
			currentMode = 1;		// Entramos en el modo 1 que dibuja l�neas
			break;
		}

		case SDLK_KP_2:
		case SDLK_2: {				// Lo mismo con el 2 y cono los siguientes n�meros, pero cada uno con su funci�n
			drawLines = false;
			drawRectangles = true;
			drawCircles = false;
			drawTriangles = false;
			currentMode = 2;
			break;
		}

		case SDLK_KP_3:
		case SDLK_3: {
			drawLines = false;
			drawRectangles = false;
			drawCircles = true;
			drawTriangles = false;
			currentMode = 3;
			break;
		}

		case SDLK_KP_4:
		case SDLK_4: {
			drawLines = false;
			drawRectangles = false;
			drawCircles = false;
			drawTriangles = true;
			currentMode = 4;
			break;
		}

		case SDLK_KP_6:
		case SDLK_6: {				// En este modo no se dibujan figuras, ya que este ser� el encargado de las part�culas
			drawLines = false;
			drawRectangles = false;
			drawCircles = false;
			drawTriangles = false;
			currentMode = 6;
			break;
		}

		case SDLK_f: {				// Este cambia el estado de relleno de las figuras que haya en pantalla en ese momento
			isFilled = !isFilled;
			break;
		}
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