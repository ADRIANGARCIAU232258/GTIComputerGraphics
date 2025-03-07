#pragma once
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "framework.h" // Para Matrix44 y otras estructuras
#include "material.h"

class Entity {
public:
    Mesh* mesh;       // Puntero a la malla de la entidad
    Shader* shader;   // Puntero al shader que usar� la entidad
    Texture* texture; // Puntero a la textura de la entidad
	Material* material; // Puntero al material de la entidad
    Matrix44 model;   // Matriz de transformaci�n del objeto

    // Constructor
    Entity();

    // M�todo de renderizado
    void Render(const sUniformData uniformData);
    void Render(Camera* camera);
    // Devuelve la matriz de transformaci�n del objeto
    Matrix44 GetModelMatrix() { return model; }

    // Destructor
    ~Entity();
};

