#pragma once

#include <vector>
#include "framework.h"
#include "camera.h"
#include "shader.h"
#include "main/includes.h"

struct sLight {
    Vector3 position;  // Posición de la luz
    Vector3 color;     // Color/intensidad de la luz
};
// Estructura para almacenar los datos uniformes
struct sUniformData
{
    Matrix44 model;          // Matriz de modelo
    Matrix44 viewprojection; // Matriz de vista-proyección
    Vector3 ambientLight;    // Intensidad de la luz ambiente
	sLight light;			// Datos de la luz
};

class Material
{
public:
    std::string name;

    Shader* matshader;
    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;
    float shininess;
    Texture* mattexture;

    Material();

    ~Material();
    void Enable(const sUniformData& uniformData); // Ahora recibe sUniformData
    void Disable();

    
};
