#include "material.h"

// Constructor por defecto
Material::Material()
{
    matshader = nullptr;
    mattexture = nullptr;
    ambient = Vector3(0.2f, 0.2f, 0.2f);  // Componente ambiental
    diffuse = Vector3(1.0f, 1.0f, 1.0f);  // Componente difusa
    specular = Vector3(1.0f, 1.0f, 1.0f); // Componente especular
    shininess = 32.0f;  // Brillo
};

    Material::~Material(){

    // Liberar recursos si es necesario
    if (mattexture) {
        delete mattexture;
        mattexture = nullptr;
    }
    if (matshader) {
        delete matshader;
        matshader = nullptr;
    };
}
void Material::Enable(const sUniformData& uniformData) {
    if (!matshader) return;  // Si no hay shader, no hacemos nada

    
    matshader->Enable();

    // Subir las propiedades del material al shader
    matshader->SetVector3("u_Ka", ambient);  
    matshader->SetVector3("u_Kd", diffuse); 
    matshader->SetVector3("u_Ks", specular); 
    matshader->SetFloat("u_shininess", shininess);  

    // Subir las propiedades de la luz al shader
    matshader->SetVector3("u_lightPosition", uniformData.light.position);
    matshader->SetVector3("u_lightColor", uniformData.light.color);
    matshader->SetVector3("u_ambientLight", uniformData.ambientLight);

    // Subir las matrices al shader
    matshader->SetMatrix44("u_model", uniformData.model);
    matshader->SetMatrix44("u_viewprojection", uniformData.viewprojection);

    // Subir la textura al shader (si existe)
    if (mattexture) {
        matshader->SetTexture("u_texture", mattexture);
    }
}

// Destructor
void Material::Disable() {
    if (matshader) {
        matshader->Disable();  // Deshabilitar el shader del material
    }
}