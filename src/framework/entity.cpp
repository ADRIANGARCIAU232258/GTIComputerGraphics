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
