#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include <fstream>
#include <iostream>
#include "texture.h"


// Shaders
const char* vertex_shader_code = R"(
varying vec2 v_uv; 
void main()
{
// Set vertex uvs
	v_uv = gl_MultiTexCoord0.xy;
// Output clip-space
	gl_Position = gl_Vertex;
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
varying vec2 v_uv; 

void main()
{
	gl_FragColor = vec4(v_uv.x, 0.0, 1.0 - v_uv.x, 1.0);
}
)";

const char* fragment_shader_code_b = R"(
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
    vec4 texture_color = texture2D( u_texture , v_uvs );
    float gray = dot(texture_color.rgb, vec3(0.299, 0.587, 0.114));
    gl_FragColor = vec4(vec3(gray), texture_color.a);
}
)";

const char* fragment_shader_code_bi = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;

void main()
{
    vec4 inverter = vec4 (1.0);
    vec4 texture_color = texture2D( u_texture , v_uvs );
    gl_FragColor = inverter-texture_color;
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

const char* fragment_shader_code_ei = R"(
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

const char* fragment_shader_code_fi = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;

void main()
{
   vec2 tex_offset = vec2(0.009); // Controla el desenfoque
    vec4 sum = texture2D(u_texture, v_uvs) * 0.4;
    sum += texture2D(u_texture, v_uvs + tex_offset) * 0.15;
    sum += texture2D(u_texture, v_uvs - tex_offset) * 0.15;
    sum += texture2D(u_texture, v_uvs + vec2(tex_offset.x, -tex_offset.y)) * 0.15;
    sum += texture2D(u_texture, v_uvs + vec2(-tex_offset.x, tex_offset.y)) * 0.15;
    
    gl_FragColor = sum;
}
)";

const char* fragment_shader_code_gi = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;
uniform vec2 pixelSize;

vec2 pixelate(vec2 coord, vec2 pixelSize) {
    return floor(coord / pixelSize) * pixelSize;
}

void main() {
    vec2 pixelated_uv = pixelate(v_uvs, pixelSize);
    vec4 texture_color = texture2D(u_texture, pixelated_uv);
    gl_FragColor = texture_color;
}
)";

const char* fragment_shader_code_hi = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;
uniform float intensity; // Control de la intensidad de la distorsión

void main()
{
    vec2 center = vec2(0.5); // Centro de la imagen
    vec2 uv = v_uvs;

    // Aplicar una distorsión fija basada en funciones seno y coseno
    uv.x += sin(uv.y * 10.0) * 0.05 * intensity; // Deformación horizontal
    uv.y += cos(uv.x * 10.0) * 0.05 * intensity; // Deformación vertical

    // Recuperar el color de la textura deformada
    vec4 texture_color = texture2D(u_texture, uv);
    gl_FragColor = texture_color;
}
)";


const char* fragment_shader_code_original = R"(
varying vec2 v_uvs;
uniform sampler2D u_texture;

void main()
{
    vec4 texture_color = texture2D(u_texture, v_uvs);
    gl_FragColor = texture_color;
}
)";

Application::Application(const char* caption, int width, int height)
    : landscapeTexture(nullptr), quadmesh(nullptr), quadshader(nullptr), 
    texmesh(nullptr), texshader(nullptr), texture(nullptr) // Inicializamos las variables miembro
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
    current_lab = 0;
}

Application::~Application()
{
}

void Application::Init(void)
{

	std::cout << "Initiating app..." << std::endl;

	quadshader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
	quadmesh = new Mesh();
    leemesh = new Mesh();
	quadmesh->CreateQuad();
	texshader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
	texture = Texture::Get("images/fruits.png");
	landscapeTexture = Texture::Get("images/landscape.png");
    mshader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
    mtexture = Texture::Get("textures/lee_color_specular.tga");
	leemesh->LoadOBJ("meshes/lee.obj");
	gourshader = Shader::Get("shaders/gouraud.vs", "shaders/gouraud.fs");
    gourmaterial = new Material();
    gourmaterial->matshader = gourshader;
    gourmaterial->ambient = Vector3(0.2f, 0.2f, 0.2f);  // Componente ambiental
    gourmaterial->diffuse = Vector3(1.0f, 1.0f, 1.0f);  // Componente difusa
    gourmaterial->specular = Vector3(1.0f, 1.0f, 1.0f); // Componente especular
    gourmaterial->shininess = 32.0f;
    camera = new Camera();
    entity = new Entity();
    entity->mesh = leemesh;
    entity->shader = mshader;
    entity->texture = mtexture;
	gourmaterial->mattexture = mtexture;
    entity->model.SetIdentity();
    entity->material = gourmaterial;
    uniformData.model = Matrix44();
	uniformData.viewprojection = Matrix44();
    
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

    texshaders.push_back(new Shader());
    texshaders.back()->CompileFromMemory(vertex_tshader_code, fragment_shader_code_gi);

    texshaders.push_back(new Shader());
    texshaders.back()->CompileFromMemory(vertex_tshader_code, fragment_shader_code_hi);

    texshaders.push_back(new Shader());
    texshaders.back()->CompileFromMemory(vertex_tshader_code, fragment_shader_code_original);
}

// Render one frame
void Application::Render(void)
{
    if (current_lab == 0) {
        if (current_exercise == 1) {
            glEnable(GL_DEPTH_TEST);
            shaders[current_shader]->Enable();
            // Declaramos la resolución de la ventana al shader
            shaders[current_shader]->SetVector2("u_resolution", Vector2(static_cast<float>(this->window_width), static_cast<float>(this->window_height)));
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
        if (current_exercise == 3) {
            glEnable(GL_DEPTH_TEST);
            if (current_shader == 6 || current_shader == 7 || current_shader == 8) { // Incluyendo el shader original
                texshaders[current_shader]->Enable();
                texshaders[current_shader]->SetTexture("u_texture", landscapeTexture);

                if (current_shader == 6) { // Pixelación
                    texshaders[current_shader]->SetVector2("pixelSize", Vector2(0.025f, 0.025f)); // Tamaño de los píxeles
                }
                if (current_shader == 7) { // Deformación
                    float intensity = 1.0 * sin(time * 1.8); // Valor para ajustar la intensidad de la deformación
                    texshaders[current_shader]->SetFloat("intensity", intensity);

                }
                quadmesh->Render();
                texshaders[current_shader]->Disable();
            }
        }
        if (current_exercise == 4) {
			glEnable(GL_DEPTH_TEST);
            mshader->Enable();
            camera->SetPerspective(45.0f, window_width / (float)window_height, 0.1f, 1000.0f);
            camera->LookAt(Vector3(0, 0.5, 1), Vector3(0, 0.25, 0), Vector3(0, 1, 0));
            mshader->SetTexture("u_texture", mtexture);
            leemesh->Render();
            entity->Render(camera);
            mshader->Disable();
        }
    }
    else {
        glEnable(GL_DEPTH_TEST);

        // Configurar la cámara
        camera->SetPerspective(45.0f, window_width / (float)window_height, 0.1f, 1000.0f);
        camera->LookAt(Vector3(0, 0.5, 1), Vector3(0, 0.25, 0), Vector3(0, 1, 0));

        // Configurar las matrices
        uniformData.model = entity->GetModelMatrix();
        uniformData.viewprojection = camera->GetViewProjectionMatrix();

        // Configurar las propiedades de la luz
        uniformData.light.position = Vector3(1.0f, 1.0f, 1.0f);  // Posición de la luz
        uniformData.light.color = Vector3(1.0f, 1.0f, 1.0f);     // Color de la luz (blanco)
        uniformData.ambientLight = Vector3(0.5f, 0.5f, 0.5f);
        // Intensidad de la luz ambiental
        gourmaterial->Enable(uniformData);
        entity->Render(uniformData);
        // Renderizar la entidad
        gourmaterial->Disable();

        glDisable(GL_DEPTH_TEST);
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
    case SDLK_KP_1: current_exercise = 1; current_shader = 0; break;
	case SDLK_2:
    case SDLK_KP_2: current_exercise = 2; current_shader = 8; break;
	case SDLK_3:
    case SDLK_KP_3: current_exercise = 3; current_shader = 8; break;
	case SDLK_4:
	case SDLK_KP_4: current_exercise = 4; break;
    case SDLK_l: current_lab = (current_lab == 0)? 1 : 0; 
    std::cout << "LAB" << ((current_lab == 0) ? "4 ACTIVO" : "5 ACTIVO") << std::endl;
        break;
    default:
        if (current_exercise == 1) {
            switch (event.keysym.sym) {
            case SDLK_a: current_shader = 0; break;
            case SDLK_b: current_shader = 1; break;
            case SDLK_c: current_shader = 2; break;
            case SDLK_d: current_shader = 3; break;
            case SDLK_e: current_shader = 4; break;
            case SDLK_f: current_shader = 5; break;
            }
        }
        else if (current_exercise == 2) {
            switch (event.keysym.sym) {
            case SDLK_a: current_shader = 0; break;
            case SDLK_b: current_shader = 1; break;
            case SDLK_c: current_shader = 2; break;
            case SDLK_d: current_shader = 3; break;
            case SDLK_e: current_shader = 4; break;
            case SDLK_f: current_shader = 5; break;
            }
        }
        else if (current_exercise == 3) {
            switch (event.keysym.sym) {
            case SDLK_a: current_shader = 6; break;
            case SDLK_b: current_shader = 7; break;
            }
        }
        break;
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