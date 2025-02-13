#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"

Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
	pixels = NULL;
	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
}

// Assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;

	if(c.pixels)
	{
		pixels = new Color[width*height*bytes_per_pixel];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}

void Image::Render()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = GetPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.SetPixelUnsafe( x, y, GetPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::FlipY()
{
	int row_size = bytes_per_pixel * width;
	Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
	for (int y = 0; y < height * 0.5; y += 1)
	{
		Uint8* pos = (Uint8*)pixels + y * row_size;
		memcpy(temp_row, pos, row_size);
		Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
		memcpy(pos, pos2, row_size);
		memcpy(pos2, temp_row, row_size);
	}
	delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
	std::string sfullPath = absResPath(filename);
	std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

	// Get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (!size)
		return false;

	std::vector<unsigned char> buffer;

	// Read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else
		buffer.clear();

	std::vector<unsigned char> out_image;

	if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0)
		return false;

	size_t bufferSize = out_image.size();
	unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);
	
	// Force 3 channels
	bytes_per_pixel = 3;

	if (originalBytesPerPixel == 3) {
		pixels = new Color[bufferSize];
		memcpy(pixels, &out_image[0], bufferSize);
	}
	else if (originalBytesPerPixel == 4) {

		unsigned int newBufferSize = width * height * bytes_per_pixel;
		pixels = new Color[newBufferSize];

		unsigned int k = 0;
		for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
			pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
			k++;
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int imageSize;
	unsigned int bytesPerPixel;

    std::string sfullPath = absResPath( filename );

	FILE * file = fopen( sfullPath.c_str(), "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << sfullPath.c_str() << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	// Save info in image
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	// Convert to float all pixels
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			// Make sure we don't access out of memory
			if( (pos < imageSize) && (pos + 1 < imageSize) && (pos + 2 < imageSize))
				SetPixelUnsafe(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::string fullPath = absResPath(filename);
	FILE *file = fopen(fullPath.c_str(), "wb");
	if ( file == NULL )
	{
		perror("Failed to open file: ");
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	// Convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[y*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);

	return true;
}


// DECLARACIÓN FUNCIÓN DRAWPIXEL PARA POSTEIORMENTE USARLO PARA LA CREACIÓN DE PARTÍCULAS
void Image::DrawPixel(int x, int y, const Color& color) {
	if (x >= 0 && x < width && y >= 0 && y < height) {
		pixels[y * width + x] = color;
	}
}

// FUNCIONES PARA INICIALIZAR, RENDERIZAR Y ACTUALIZAR LAS PARTÍCULAS POR PANTALLA
void ParticleSystem::Init() {
	srand(static_cast<unsigned>(time(0))); // Inicializamos la generación de números aleatorios
	for (int i = 0; i < MAX_PARTICLES; ++i) { // Creamos un loop con todas las partículas
		particles[i].position = { static_cast<float>(rand() % 2560), static_cast<float>(rand() % 1369) }; // Asignamos una posición aleatoria a la partícula en un margen de 2560x1369
		particles[i].velocity = { 0.0f, static_cast<float>(-(rand() % 5 + 1) * 200) }; // Asignamos una velocidad aleatoria a la partícula
		int colorChoice = rand() % 3; // Elegimos un color aleatorio entre tres opciones
		if (colorChoice == 0) {
			particles[i].color = Color(255, 255, 255); // Blanco
		}
		else if (colorChoice == 1) {
			particles[i].color = Color(0, 14, 255); // Azul
		}
		else {
			particles[i].color = Color(132, 0, 255); // Morado
		}
		particles[i].acceleration = 0.0f; // Ponemos la aceleración de la partícula a 0
		particles[i].ttl = static_cast<float>(rand() % 100 + 50); // Asignamos un tiempo de vida aleatorio a la partícula
		particles[i].inactive = false; // Marcamos la partícula como activa
		particles[i].size = static_cast<float>(rand() % 3 + 1); // Asignamos un tamaño aleatorio a la partícula entre 1 y 3 (para dar variedad a la imagen)
	}
}

void ParticleSystem::Render(Image* framebuffer) {
	for (int i = 0; i < MAX_PARTICLES; ++i) { // Volvemos a usar el loop para todas las partículas
		if (!particles[i].inactive) { // Si la partícula está activa...
			for (int dx = -particles[i].size; dx <= particles[i].size; ++dx) { // Iteramos sobre el tamaño de la partícula en el eje x
				for (int dy = -particles[i].size; dy <= particles[i].size; ++dy) { // Iteramos sobre el tamaño de la partícula en el eje y
					framebuffer->DrawPixel(static_cast<int>(particles[i].position.x + dx), static_cast<int>(particles[i].position.y + dy), particles[i].color); // Aquí dibujamos el píxel de la partícula en el framebuffer
				}
			}
		}
	}
}

void ParticleSystem::Update(float dt) {
	for (int i = 0; i < MAX_PARTICLES; ++i) { // De nuevo el mismo loop que las dos funciones anteriores
		if (!particles[i].inactive) { // Si la partícula está activa...
			particles[i].position.x += particles[i].velocity.x * dt; // Actualizamos la posición de la partícula en el eje x
			particles[i].position.y += particles[i].velocity.y * dt; // Actualizamos la posición de la partícula en el eje y
			particles[i].ttl -= dt; // Reducimos el tiempo de vida de la partícula
			if (particles[i].ttl <= 0 || particles[i].position.y < 0) { // Si el tiempo de vida de la partícula ha expirado o la partícula ha salido de la pantalla...
				particles[i].inactive = true; // Marcamos la partícula como inactiva
				// Reiniciamos la partícula
				particles[i].position = { static_cast<float>(rand() % 2560), static_cast<float>(rand() % 1369) }; // Generamos una nueva posición para la partícula desde la parte superior
				particles[i].velocity = { 0.0f, static_cast<float>(-(rand() % 5 + 1) * 200) }; // Asignamos una nueva velocidad a la partícula
				int colorChoice = rand() % 3; // Elegimos un nuevo color aleatorio para la partícula
				if (colorChoice == 0) {
					particles[i].color = Color(255, 255, 255); // Blanco
				}
				else if (colorChoice == 1) {
					particles[i].color = Color(0, 14, 255); // Azul
				}
				else {
					particles[i].color = Color(132, 0, 255); // Morado
				}
				particles[i].ttl = static_cast<float>(rand() % 100 + 50); // Asignamos un nuevo tiempo de vida a la partícula
				particles[i].inactive = false; // Marcamos la partícula como activa
				particles[i].size = static_cast<float>(rand() % 3 + 1); // Asignamos un nuevo tamaño a la partícula entre 1 y 3
			}
		}
	}
}


// FUNCIÓN PARA DIBUJAR LÍNEAS CON EL MÉTODO DDA
void Image::DrawLineDDA(int x0, int y0, int x1, int y1, const Color& c) {

	int dx = x1 - x0;		// Calculamos las coordenadas del vector director entre p0 y p1
	int dy = y1 - y0;

	int steps = std::max(abs(dx), abs(dy));			// Calculamos los pasos necesarios para crear una línea diagonal

	float incrementX = (float)dx / (float)steps;			// Calculamos la dirección para avanzar en cada iteración
	float incrementY = (float)dy / (float)steps;

	float x = (float)x0;		// Inicializamos las coordenadas
	float y = (float)y0;

	for (int i = 0; i <= steps; i++) {
		SetPixel((unsigned int)round(x), (unsigned int)round(y), c);	// Aquí dibujamos el píxel en las coordenadas actuales

		x += incrementX;		// Tenemos que incrementar las coordenadas para pintar cada píxel de la línea
		y += incrementY;
	}
}


// FUNCIÖN PARA DIBUJAR RECTÁNGULOS
void Image::DrawRect(int x, int y, int w, int h, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor)
{
	for (int i = 0; i < borderWidth; ++i)	// Esto sirve para pintar el borde del rectángulo
	{
		// Pintamos las líneas horizontales del rectángulo
		DrawLineDDA(x -	i, y - i, x + w + i, y - i, borderColor); 
		DrawLineDDA(x - i, y + h + i, x + w + i, y + h + i, borderColor);

		// Después pintamos las verticales
		DrawLineDDA(x - i, y - i, x - i, y + h + i, borderColor);
		DrawLineDDA(x + w + i, y - i, x + w + i, y + h + i, borderColor); 
	}

	// Completamos el interior del rectángulo en caso que la booleana isFilled sea True
	if (isFilled)
	{
		for (int i = x; i < x + w; ++i)
		{
			for (int j = y; j < y + h; ++j)
			{
				SetPixel(i, j, fillColor);
			}
		}
	}
}

// FUNCIÓN PARA ESCANEAR LAS LÍNEAS DE UN TRIÁNGULO
void Image::ScanLineDDA(int x0, int y0, int x1, int y1, std::vector<Cell>& table) {
	
	int dx = x1 - x0;		// Calculamos las coordenadas del vector director entre p0 y p1
	int dy = y1 - y0;

	int steps = std::max(abs(dx), abs(dy));			// Calculamos los pasos necesarios para crear una línea diagonal

	float incrementX = (float)dx / (float)steps;			// Calculamos la dirección para avanzar en cada iteración
	float incrementY = (float)dy / (float)steps;

	float x = (float)x0;		// Inicializamos las coordenadas
	float y = (float)y0;

	for (int i = 0; i <= steps; i++) {		// Iteramos sobre cada paso
		int currentY = (int)y;				// Convertimos Y a un número entero
		if (currentY >= 0 && currentY < table.size()) {			// Necesitamos comprobar si currentY se encuentra dentro de los límites de la tabla
			table[currentY].minX = std::min(table[currentY].minX, (int)x);		// En caso de que eso pase entonces actualizamos minX y maxX para cada fila de la tabla
			table[currentY].maxX = std::max(table[currentY].maxX, (int)x);
		}
		x += incrementX;
		y += incrementY;
	}
}

// FUNCIÓN PARA DIBUJAR TRIÁNGULOS
void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor) {
	std::vector<Cell> table(height);	// Creamos una tabla con el mismo número de filas que la altura de la imagen

	int x0 = (int)(p0.x);				// Estas son las coordenadas x e y de los tres puntos que frman el triángulo
	int y0 = (int)(p0.y);
	int x1 = (int)(p1.x);
	int y1 = (int)(p1.y);
	int x2 = (int)(p2.x);
	int y2 = (int)(p2.y);

	ScanLineDDA(x0, y0, x1, y1, table);		// Rasterizamos los bordes del triángulo y actualizamos la tabla
	ScanLineDDA(x1, y1, x2, y2, table);
	ScanLineDDA(x2, y2, x0, y0, table);

	SetPixel(x0, y0, borderColor);			// Esto pinta el borde
	SetPixel(x1, y1, borderColor);
	SetPixel(x2, y2, borderColor);
	DrawLineDDA(x0, y0, x1, y1, borderColor);
	DrawLineDDA(x1, y1, x2, y2, borderColor);
	DrawLineDDA(x0, y0, x2, y2, borderColor);

	if (isFilled) {			// Si queremos rellenar el triángulo...
		for (unsigned int y = 0; y < height; y++) {			// Iteramos entre la base y la altura del triángulo
			if (table[y].minX <= table[y].maxX) {			// Si minX es menor que maxX...	
				for (int x = table[y].minX; x <= table[y].maxX; x++) {		// Iteramos entre minX y maxX para cada fila
					SetPixel(x, y, fillColor);		// Le damos color al píxel entre minX y maxX
				}
			}
		}
	}
}

// FUNCIÓN PARA DIBUJAR CÍRCULOS
void Image::DrawCircle(int x0, int y0, int r, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor)
{
	for (int i = 0; i < borderWidth; ++i)		// Aquí dibujamos el borde del círculo
	{
		MidpointCircle(x0, y0, r + i, borderColor);
	}

	if (isFilled)		// Si es necesario lo rellenamos
	{
		for (int i = 0; i < r; ++i)
		{
			MidpointCircleFill(x0, y0, i, fillColor);
		}
	}
}

// ALGORITMO PARA DIBUJAR UN CÍRCULO
void Image::MidpointCircle(int x0, int y0, int r, const Color& color)
{
	int x = r;			// Inicializamos x para el radio
	int y = 0;			// Ini// Inicializamos y a 0
	int p = 1 - r;		// Parámetro de decisión inicial

	while (x >= y)
	{
		SetPixel(x0 + x, y0 + y, color);		// Dibujamos los 8 puntos simétricos del círculo
		SetPixel(x0 - x, y0 + y, color);
		SetPixel(x0 + x, y0 - y, color);
		SetPixel(x0 - x, y0 - y, color);
		SetPixel(x0 + y, y0 + x, color);
		SetPixel(x0 - y, y0 + x, color);
		SetPixel(x0 + y, y0 - x, color);
		SetPixel(x0 - y, y0 - x, color);

		y++;		// Incrementamos y

		if (p <= 0)
		{
			p = p + 2 * y + 1;		// Si p <= 0 actualizamos el parámetro de decisión
		}
		else
		{
			x--;	// Reducimos x
			p = p + 2 * y - 2 * x + 1;		// Si p > 0 actualizamos el parámetro de decisión
		}
	}
}

// ALGORITMO PARA RELLENAR UN CÍRCULO
void Image::MidpointCircleFill(int x0, int y0, int r, const Color& color)
{
	int x = r;			// Inicializamos x para el radio
	int y = 0;			// Ini// Inicializamos y a 0
	int p = 1 - r;		// Parámetro de decisión inicial

	while (x >= y)
	{
		DrawLineDDA(x0 - x, y0 + y, x0 + x, y0 + y, color);			// Dibujamos las líneas horizontales para rellenar el círculo
		DrawLineDDA(x0 - x, y0 - y, x0 + x, y0 - y, color);
		DrawLineDDA(x0 - y, y0 + x, x0 + y, y0 + x, color);
		DrawLineDDA(x0 - y, y0 - x, x0 + y, y0 - x, color);

		y++;		// Incrementamos y

		if (p <= 0)
		{
			p = p + 2 * y + 1;		// Si p <= 0 actualizamos el parámetro de decisión
		}
		else
		{
			x--;	// Reducimos x
			p = p + 2 * y - 2 * x + 1;		// Si p > 0 actualizamos el parámetro de decisión
		}
	}
}

void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2) {
	// Calcular el área del triángulo completo
	float area = (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);

	// Iterar sobre todos los píxeles dentro del triángulo
	for (int y = std::min(p0.y, std::min(p1.y, p2.y)); y <= std::max(p0.y, std::max(p1.y, p2.y)); y++) {
		for (int x = std::min(p0.x, std::min(p1.x, p2.x)); x <= std::max(p0.x, std::max(p1.x, p2.x)); x++) {

			// Calcular las áreas de los subtriángulos
			float area0 = (p1.x - x) * (p2.y - y) - (p2.x - x) * (p1.y - y);
			float area1 = (p2.x - x) * (p0.y - y) - (p0.x - x) * (p2.y - y);
			float area2 = (p0.x - x) * (p1.y - y) - (p1.x - x) * (p0.y - y);

			// Normalizar las áreas para obtener las coordenadas baricéntricas
			float u = area0 / area;
			float v = area1 / area;
			float w = area2 / area;

			// Si el píxel está dentro del triángulo (todas las áreas deben ser >= 0)
			if (u >= 0 && v >= 0 && w >= 0) {
				// Interpolar el color utilizando las coordenadas baricéntricas
				Color finalColor = c0 * u + c1 * v + c2 * w;

				// Dibujar el píxel con el color interpolado
				SetPixel(x, y, finalColor); // Asume que hay una función SetPixel(x, y, color)
			}
		}
	}
}








#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width * height];
	memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height * sizeof(float)];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width * height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = GetPixel(x, y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}