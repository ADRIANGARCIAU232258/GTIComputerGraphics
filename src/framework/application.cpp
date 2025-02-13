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
int current_property = 0; // Usaremos 0 para nada, 1 para near, 2 para far y 3 para FOV

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

	// Configuramos la cámara inicialmente
	camera.SetPerspective(camera_fov * DEG2RAD, (float)window_width / window_height, camera_near, camera_far);
	camera_target = Vector3(0, 0, 0);
	camera_distance = 5.0f; // Distancia inicial de la cámara
	camera.LookAt(Vector3(0, 0, camera_distance), camera_target, Vector3(0, 1, 0));
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

	// Establecemos las posiciones iniciales de las entidades para evitar que se solapen
	entities[0]->modelMatrix.SetTranslation(-2, 0, 0);
	entities[1]->modelMatrix.SetTranslation(0, 0, 0);
	entities[2]->modelMatrix.SetTranslation(2, 0, 0);
}

// Render one frame
void Application::Render(void)
{
	framebuffer.Fill(Color::BLACK);

	// Lista de colores para cada una de las entidades
	Color colors[] = { Color::WHITE, Color::YELLOW, Color::BLUE };

	// Renderizamos las entidades según el modo actual
	if (currentMode == 1) {
		// Dibujamos una sola entidad
		if (!entities.empty()) {
			entities[0]->Render(&framebuffer, &camera, colors[0]);
		}
	}
	else if (currentMode == 2) {
		// Dibujamos múltiples entidades con animaciçon
		for (size_t i = 0; i < entities.size(); ++i) {
			entities[i]->Render(&framebuffer, &camera, colors[i]);
		}
	}

	framebuffer.Render(); // Finalmente se va renderizando la imagen
}

// Called after render
void Application::Update(float dt)
{
	// Controlamos las propiedades de la cámara
	if (current_property == 1) {
		camera_near += 1.0f * dt;
		if (camera_near >= camera_far) {		// Para evitar números fuera del rango, establecemos ciertas limitaciones
			camera_near = camera_far - 0.1f;
		}
	}
	else if (current_property == 2) {
		camera_far += 1.0f * dt;
		if (camera_far <= camera_near) {
			camera_far = camera_near + 0.1f;
		}
	}
	else if (current_property == 3) {
		camera_fov += 0.1f * dt;
		if (camera_fov < 1.0f) {
			camera_fov = 1.0f;
		}
		else if (camera_fov > 179.0f) {
			camera_fov = 179.0f;
		}
	}
	
	if (currentMode == 2) {
		// Asignamos animaciones distintas a cada entidad
		float angles[] = { 90.0f, 45.0f, 60.0f };
		for (size_t i = 0; i < entities.size(); ++i) {
			float angle = angles[i] * dt; // Para animarlas de manera distintas, asignaremos un angulo de rotación diferente para cada una de las entidades
			Matrix44 rotationMatrix;
			rotationMatrix.SetRotation(angle * DEG2RAD, Vector3(0, 1, 0));	// La rotación será respecto el eje Y
			entities[i]->modelMatrix = rotationMatrix * entities[i]->modelMatrix; // Aplicamos la rotación al modelo
		}
	}

	// Actualizamos las matrices de la cámara
	camera.SetPerspective(camera_fov * DEG2RAD, (float)window_width / window_height, camera_near, camera_far);
	camera.UpdateProjectionMatrix();
	camera.LookAt(Vector3(0, 0, camera_distance), camera_target, Vector3(0, 1, 0));
	camera.UpdateViewMatrix();
}

//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym) {
		case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app

		case SDLK_1:
		case SDLK_KP_1:
			currentMode = 1; // Dibujamos una sola entidad
			std::cout << "Modo actual: Dibuja una sola entidad" << std::endl;
			break;
		case SDLK_2:
		case SDLK_KP_2:
			currentMode = 2; // Dibujamos varias entidades animadas
			std::cout << "Modo actual: Dibuja múltiples entidades animadas" << std::endl;
			break;
		case SDLK_n:
			current_property = 1; // Configuramos la propiedad actual a near
			std::cout << "Propiedad actual: Near" << std::endl;
			break;
		case SDLK_f:
			current_property = 2; // Configuramos la propiedad actual a far
			std::cout << "Propiedad actual: Far" << std::endl;
			break;
		case SDLK_v:
			current_property = 3; // Configuramos la propiedad actual a fov
			std::cout << "Propiedad actual: FOV" << std::endl;
			break;
		case SDLK_PLUS:
		case SDLK_KP_PLUS:
			// Incrementamos la propiedad actual
			if (current_property == 1) {
				camera_near += 1.0f;
				if (camera_near >= camera_far) {
					camera_near = camera_far - 0.1f;
				}
			}
			else if (current_property == 2) {
				camera_far += 1.0f;
				if (camera_far <= camera_near) {
					camera_far = camera_near + 0.1f;
				}
			}
			else if (current_property == 3) {
				camera_fov += 1.0f;
				if (camera_fov < 1.0f) {
					camera_fov = 1.0f;
				}
				else if (camera_fov > 179.0f) {
					camera_fov = 179.0f;
				}
			}
			break;
		case SDLK_MINUS:
		case SDLK_KP_MINUS:
			// Disminuimos la propiedad actual
			if (current_property == 1) {
				camera_near -= 1.0f;
				if (camera_near < 0.1f) {
					camera_near = 0.1f;
				}
			}
			else if (current_property == 2) {
				camera_far -= 1.0f;
				if (camera_far < 0.1f) {
					camera_far = 0.1f;
				}
			}
			else if (current_property == 3) {
				camera_fov -= 1.0f;
				if (camera_fov < 1.0f) {
					camera_fov = 1.0f;
				}
				else if (camera_fov > 179.0f) {
					camera_fov = 179.0f;
				}
			}
			break;
	}

	// Actualizamos las matrices de la cámara
	camera.SetPerspective(camera_fov * DEG2RAD, (float)window_width / window_height, camera_near, camera_far);
	camera.UpdateProjectionMatrix();
	camera.UpdateViewMatrix();
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