#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include <fstream>
#include <iostream>
#include "texture.h"


// Shaders
const char* vertex_shader_code = R"(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(position, 1.0);
    TexCoord = texCoord;
}

)";

const char* vertex_tshader_code = R"(
varying vec2 v_uvs; 
void main()
{
// Set vertex uvs
	v_uvs = gl_MultiTexCoord0.xy;
// Output clip-space
	gl_Position = gl_Vertex;
}

)";

const char* fragment_shader_code_a = R"(
#version 330 core
out vec4 FragColor;

uniform vec2 u_resolution;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution; // Usamos las dimensiones del framebuffer
    vec3 color = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), uv.x); // Interpolamos entre azul y rojo
    FragColor = vec4(color, 1.0);
}
)";

const char* fragment_shader_code_b = R"(
#version 330 core

out vec4 FragColor;

uniform vec2 u_resolution;

void main()
{
    vec2 uv = (gl_FragCoord.xy / u_resolution) * 2.0 - 1.0; // Normalizamos a [-1, 1]
    float dist = length(uv)/sqrt(2.0); // Calculamos la distancia desde el centro. Dividimos entre la distancia máxima para que el máximo sea 1.0.
    FragColor = vec4(vec3(dist), 1.0); // Usamos la distancia para el color
}

)";

const char* fragment_shader_code_c = R"(
#version 330 core
out vec4 FragColor;

uniform vec2 u_resolution;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;
    float gridSize = 50.0; // Tamaño de la cuadrícula
    float lineThickness = 0.5; // Grosor de la línea 
    float blurAmount = 0.2; // Controla qué tan suave es la transición al negro

    // Crear líneas rojas verticales con bordes suavizados
    float redLines = smoothstep(lineThickness, lineThickness + blurAmount, mod(gl_FragCoord.x, gridSize) / gridSize);

    // Crear líneas azules horizontales con bordes suavizados
    float blueLines = smoothstep(lineThickness, lineThickness + blurAmount, mod(gl_FragCoord.y, gridSize) / gridSize);

    // Combinar las líneas sobre un fondo negro
    vec3 color = vec3(redLines, 0.0, blueLines);

    FragColor = vec4(color, 1.0);
}
)";

const char* fragment_shader_code_d = R"(
#version 330 core
out vec4 FragColor;

uniform vec2 u_resolution;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;

    // Hacer que la cuadrícula sea cuadrada
    float gridSize = 0.04; // Ajusta para cambiar el tamaño de las celdas
    vec2 gridUV = floor(uv / gridSize) * gridSize;

    // Normalizar gridUV para que vaya de 0 a 1 en toda la pantalla
    vec2 normGridUV = gridUV / vec2(1.0);

    // Definir los colores interpolados
    vec3 color = mix(
        mix(vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), normGridUV.x), // Interpolación entre negro y rojo
        mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0), normGridUV.x), // Interpolación entre verde y amarillo
        normGridUV.y // Interpolación entre las dos filas de colores
    );

    FragColor = vec4(color, 1.0);
}
)";

const char* fragment_shader_code_e = R"(
#version 330 core
out vec4 FragColor;

uniform vec2 u_resolution;

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;
    float checker = mod(floor(gl_FragCoord.x / 50.0) + floor(gl_FragCoord.y / 50.0), 2.0);
    FragColor = vec4(vec3(checker), 1.0);
}
)";

const char* fragment_shader_code_f = R"(
#version 330 core
out vec4 FragColor;

uniform vec2 u_resolution;

void main()
{
    // Coordenadas normalizadas
    vec2 uv = gl_FragCoord.xy / u_resolution;

    // Onda seno centrada en la pantalla (de izquierda a derecha)
    float wave = sin(uv.x * 2.0 * 3.14159) * 0.25 + 0.5;  // Onda que varía entre 0 y 1

    // Gradiente superior (verde en el centro, negro en los bordes)
    float gradientTop = uv.y;  // Degradado de negro (arriba) a verde (centro)
    // Gradiente inferior (verde en el centro, negro en los bordes)
    float gradientBottom = 1.0 - uv.y;  // Degradado de negro (abajo) a verde (centro)

    // Aplicar la onda para afectar los colores verticalmente en función de la posición de la onda
    float colorTop = mix(0.0, 1.0, gradientTop);  // Negro a verde (parte superior)
    float colorBottom = mix(0.0, 1.0, gradientBottom);  // Negro a verde (parte inferior)

    // Utilizar step() para sustituir el if, determinando si estamos por encima o por debajo de la onda
    float isAboveWave = step(wave, uv.y);  // 1.0 si uv.y > wave (parte inferior), 0.0 si uv.y <= wave (parte superior)

    // Combinar los colores en función de la posición
    FragColor = vec4(0.0, mix(colorTop, colorBottom, isAboveWave), 0.0, 1.0);
}
)";

const char* fragment_shader_code_ai = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;

void main()
{
    vec4 inverter = vec4 (1.0);
    vec4 texture_color = texture2D( u_texture , v_uvs );
    gl_FragColor = inverter-texture_color;
}
)";

const char* fragment_shader_code_bi = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;

void main()
{
    vec4 texture_color = texture2D( u_texture , v_uvs );
    float gray = dot(texture_color.rgb, vec3(0.299, 0.587, 0.114));
    gl_FragColor = vec4(vec3(gray), texture_color.a);
}
)";

const char* fragment_shader_code_ci = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;

void main()
{
    vec4 texture_color = texture2D(u_texture, v_uvs);
    float gray = dot(texture_color.rgb, vec3(0.299, 0.587, 0.114)); // Convertimos a escala de grises
    vec3 yellow_tone = vec3(1.0, 1.0, 0.0); 
    gl_FragColor = vec4(gray * yellow_tone, texture_color.a);
}

)";

const char* fragment_shader_code_di = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;

void main()
{
    float blurSize = 0.005; // Valor arbitrario de desenfoque

    float kernel[5] = float[](0.06136, 0.24477, 0.38774, 0.24477, 0.06136);
    vec4 sum = vec4(0.0);

    for (int i = -2; i <= 2; i++) {
        vec2 offset = vec2(float(i) * blurSize, 0.0); 
        sum += texture2D(u_texture, v_uvs + offset) * kernel[i + 2];
    }

    gl_FragColor = sum;
}
)";

const char* fragment_shader_code_ei = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;
uniform float threshold; // Ajustable (ejemplo: 0.5 para un corte medio)

void main()
{
    vec4 texture_color = texture2D(u_texture, v_uvs);
    float gray = dot(texture_color.rgb, vec3(0.299, 0.587, 0.114));
    float threshold = 0.5;
    // Umbral aplicado directamente a cada canal de color
    vec3 bw = step(vec3(threshold), vec3(gray)); 
    
    gl_FragColor = vec4(bw, texture_color.a);
})";

const char* fragment_shader_code_fi = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;

void main()
{
    vec2 center = vec2(0.5);
    float dist = length(v_uvs - center); // Distancia al centro
    vec4 texture_color = texture2D( u_texture , v_uvs );
    gl_FragColor = texture_color*(1-dist);
}
)";

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);
	this->framebuffer.Resize(w, h);

	current_shader = 0;
	current_exercise = 0;
}

Application::~Application()
{
}

void Application::Init(void)
{
	Image* fruit = new Image();
	if (!fruit->LoadPNG("images/fruits.png", true)) {
		std::cerr << "Error: No se pudo cargar la imagen." << std::endl;
		return;
	}
	std::cout << "Initiating app..." << std::endl;
	quadshader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
	quadmesh = new Mesh();
	quadmesh->CreateQuad();
	texshader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
	texture = Texture::Get("images/fruits.png");


	// Compilamos todos los shaders y los guardamos en un vector

	shaders.push_back(new Shader());
	shaders.back()->CompileFromMemory(vertex_shader_code, fragment_shader_code_a);

	shaders.push_back(new Shader());
	shaders.back()->CompileFromMemory(vertex_shader_code, fragment_shader_code_b);

	shaders.push_back(new Shader());
	shaders.back()->CompileFromMemory(vertex_shader_code, fragment_shader_code_c);

	shaders.push_back(new Shader());
	shaders.back()->CompileFromMemory(vertex_shader_code, fragment_shader_code_d);

	shaders.push_back(new Shader());
	shaders.back()->CompileFromMemory(vertex_shader_code, fragment_shader_code_e);

	shaders.push_back(new Shader());
	shaders.back()->CompileFromMemory(vertex_shader_code, fragment_shader_code_f);

	texshaders.push_back(new Shader());
	texshaders.back()->CompileFromMemory(vertex_tshader_code, fragment_shader_code_ai);

	texshaders.push_back(new Shader());
	texshaders.back()->CompileFromMemory(vertex_tshader_code, fragment_shader_code_bi);

	texshaders.push_back(new Shader());
	texshaders.back()->CompileFromMemory(vertex_tshader_code, fragment_shader_code_ci);

	texshaders.push_back(new Shader());
	texshaders.back()->CompileFromMemory(vertex_tshader_code, fragment_shader_code_di);

	texshaders.push_back(new Shader());
	texshaders.back()->CompileFromMemory(vertex_tshader_code, fragment_shader_code_ei);

	texshaders.push_back(new Shader());
	texshaders.back()->CompileFromMemory(vertex_tshader_code, fragment_shader_code_fi);



}

// Render one frame
void Application::Render(void)
{
	if (current_exercise == 1) {
		glEnable(GL_DEPTH_TEST);
		shaders[current_shader]->Enable();
		// Declaramos la resolución de la ventana al shader
		shaders[current_shader]->SetVector2("u_resolution", Vector2(this->window_width, this->window_height));
		quadmesh->Render();
		shaders[current_shader]->Disable();
	}
	if (current_exercise == 2) {
		glEnable(GL_DEPTH_TEST);
		texshaders[current_shader]->Enable();
		texshaders[current_shader]->SetTexture("u_texture", texture);
		quadmesh->Render();
		texshaders[current_shader]->Disable();
	}
}

// Called after render
void Application::Update(float seconds_elapsed)
{

}

//keyboard press event 
void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
	case SDLK_ESCAPE: exit(0); break;
	case SDLK_1:
	case SDLK_KP_1: current_exercise = 1; break;
	case SDLK_2:
	case SDLK_KP_2: current_exercise = 2; break;
	case SDLK_3:
	case SDLK_KP_3: current_exercise = 3; break;
	case SDLK_4:
	case SDLK_KP_4: current_exercise = 4; break;
	case SDLK_a: current_shader = 0; break;
	case SDLK_b: current_shader = 1; break;
	case SDLK_c: current_shader = 2; break;
	case SDLK_d: current_shader = 3; break;
	case SDLK_e: current_shader = 4; break;
	case SDLK_f: current_shader = 5; break;
	}
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
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