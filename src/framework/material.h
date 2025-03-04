#pragma once

#include <vector>
#include "framework.h"
#include "camera.h"
#include "shader.h"
#include "main/includes.h"


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
	
	Material(const char* name, const Vector3& ambient, const Vector3& diffuse, const Vector3& specular, float shininess, Texture* texture = NULL);
	
	~Material();
	void Enable();
	void Disable();
};