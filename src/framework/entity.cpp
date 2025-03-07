#include "entity.h"

Entity::Entity() {
    mesh = nullptr;
    shader = nullptr;
    texture = nullptr;
    model.SetIdentity(); // La model matrix empieza en la identidad
}
Entity::~Entity() {
}
void Entity::Render(Camera* camera) {
    if (!mesh || !shader) return; // Asegurar que la malla y el shader existen

    shader->Enable(); // Activar el shader

    // Subir las matrices al shader
    Matrix44 view_projection_matrix = camera->GetViewProjectionMatrix();
    shader->SetMatrix44("u_model", model);
    shader->SetMatrix44("u_viewprojection", view_projection_matrix);

    // Vincular textura si existe
    if (texture) {
        shader->SetTexture("u_texture", texture);
        texture->Bind();
    }

    // Renderizar la mesh
    mesh->Render(GL_TRIANGLES);

    shader->Disable(); // Desactivar el shader
}
void Entity::Render(const sUniformData uniformData) {
    if (!mesh || !shader) return;  // Usamos el shader de la entidad, no el del material

    shader->Enable();  // Habilitamos el shader de la entidad

    // Subir las matrices al shader
    shader->SetMatrix44("u_model", uniformData.model);
    shader->SetMatrix44("u_viewprojection", uniformData.viewprojection);

    // Subir las propiedades de la luz al shader (si es necesario)
    shader->SetVector3("u_lightPosition", uniformData.light.position);
    shader->SetVector3("u_lightColor", uniformData.light.color);
    shader->SetVector3("u_ambientLight", uniformData.ambientLight);

    // Subir la textura al shader (si es necesario)
    if (texture) {
        shader->SetTexture("u_texture", texture);
    }

    // Renderizar la malla
    mesh->Render(GL_TRIANGLES);

    shader->Disable();  // Deshabilitamos el shader de la entidad
}