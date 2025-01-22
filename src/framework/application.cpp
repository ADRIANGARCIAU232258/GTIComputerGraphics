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

	borderWidth = 1;		// WE INITIALIZE THE BORDERWIDTH TO 1
}

Application::~Application()
{
}

void Application::Init(void)
{
	std::cout << "Initiating app..." << std::endl;
	//particleSystem.Init();
}

// Render one frame
void Application::Render(void)
{

	/*
	int x = 650;		// We have to define X and Y variables to start
	int y = 300;
	Color color = Color::YELLOW;

	framebuffer.Fill(Color::BLACK);
	framebuffer.DrawLineDDA(x, y, x + 100 * cos(time), y + 100 * sin(time), color);

	framebuffer.DrawRect(500, 500, 200, 100, Color(255, 0, 0), borderWidth, true, Color(0, 255, 0));
	
	
	framebuffer.Fill(Color::BLACK);		// We define the value to draw a triangle
	Vector2 p0 = { 450, 200 };
	Vector2 p1 = { 600, 375 };
	Vector2 p2 = { 400, 450 };
	Color borderColor = Color::GREEN;
	Color fillColor = Color::CYAN;		// With that we can change the fillColor of the triangle
	bool isFilled = true;

	framebuffer.DrawTriangle(p0, p1, p2, borderColor, isFilled, fillColor);
	*/
	/*framebuffer.Fill(Color::BLACK);
	particleSystem.Render();*/

	/*framebuffer.Fill(Color::BLACK);*/

	framebuffer.Render();

	framebuffer.Fill(Color::BLACK);

	if (drawLines) {
		framebuffer.DrawLineDDA(300, 300, 400, 400, Color::WHITE);
	}
	else if (drawRectangles) {
		framebuffer.DrawRect(200, 200, 100, 100, Color::RED, borderWidth, isFilled, Color::GREEN);
	}
	else if (drawCircles) {
		framebuffer.DrawCircle(500, 500, 100, Color::YELLOW, borderWidth, isFilled, Color::PURPLE);
	}
	else if (drawTriangles) {
		Vector2 p0 = { 450, 200 };
		Vector2 p1 = { 600, 375 };
		Vector2 p2 = { 400, 450 };

		framebuffer.DrawTriangle(p0, p1, p2, Color::BLUE, isFilled, Color::CYAN);
	}
	else {
		particleSystem.Render(&framebuffer);
	}
}

// Called after render
void Application::Update(float dt)
{
	//particleSystem.Update(dt);
}

//keyboard press event 
void Application::OnKeyPressed( SDL_KeyboardEvent event )
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app

		case SDLK_PLUS:
		case SDLK_KP_PLUS: borderWidth++; break;	// TO INCREMENT THE BORDER WIDTH

		case SDLK_MINUS:
		case SDLK_KP_MINUS: borderWidth = std::max(1, borderWidth - 1); break;		// TO REDUCE THE WIDTH BUT WE MAKE SURE IS HIGHER THAN 1

		case SDLK_KP_1:
		case SDLK_1: {
			drawLines = true;
			drawRectangles = false;
			drawCircles = false;
			drawTriangles = false;
			break;
		}

		case SDLK_KP_2:
		case SDLK_2: {
			drawLines = false;
			drawRectangles = true;
			drawCircles = false;
			drawTriangles = false;
			break;
		}

		case SDLK_KP_3:
		case SDLK_3: {
			drawLines = false;
			drawRectangles = false;
			drawCircles = true;
			drawTriangles = false;
			break;
		}

		case SDLK_KP_4:
		case SDLK_4: {
			drawLines = false;
			drawRectangles = false;
			drawCircles = false;
			drawTriangles = true;
			break;
		}

		case SDLK_KP_6:
		case SDLK_6: {
			drawLines = false;
			drawRectangles = false;
			drawCircles = false;
			drawTriangles = false;
			particleSystem.Init();
			break;
		}

		case SDLK_f: {
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