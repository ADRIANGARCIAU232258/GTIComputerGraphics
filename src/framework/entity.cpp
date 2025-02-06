
#include "entity.h"
#include "image.h"
#include "camera.h"

Entity::Entity() : mesh(nullptr), modelMatrix(Matrix44()) {}

Entity::Entity(Mesh* mesh, Matrix44 modelMatrix) : mesh(mesh), modelMatrix(modelMatrix) {}

void Entity::setMesh(Mesh* newMesh) {
    mesh = newMesh;
}

void Entity::setModelMatrix(Matrix44 newModelMatrix) {
    modelMatrix = newModelMatrix;
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
    if (!mesh || !framebuffer || !camera)
        return;

    // Obtener los vértices de la malla
    std::vector<Vector3> vertices = mesh->GetVertices();

    // Iterar sobre los vértices y proyectarlos
    for (size_t i = 0; i < vertices.size(); i += 3) {
        Vector3 v0 = vertices[i];
        Vector3 v1 = vertices[i + 1];
        Vector3 v2 = vertices[i + 2];

        // Transformar los vértices del espacio local al espacio mundial usando la matriz de modelo
        v0 = modelMatrix * v0;
        v1 = modelMatrix * v1;
        v2 = modelMatrix * v2;

        // Proyectar los vértices del espacio mundial al espacio de clip usando la cámara
        bool negZ0 = false, negZ1 = false, negZ2 = false;
        v0 = camera->ProjectVector(v0, negZ0);
        v1 = camera->ProjectVector(v1, negZ1);
        v2 = camera->ProjectVector(v2, negZ2);

        // Verificar si los triángulos proyectados están dentro del cubo [-1,1]^3
        if (negZ0 || negZ1 || negZ2 || abs(v0.x) > 1 || abs(v0.y) > 1 || abs(v0.z) > 1 ||
            abs(v1.x) > 1 || abs(v1.y) > 1 || abs(v1.z) > 1 || abs(v2.x) > 1 || abs(v2.y) > 1 || abs(v2.z) > 1) {
            continue;
        }

        // Convertir las posiciones del espacio de clip al espacio de pantalla
        v0.x = (v0.x * 0.5 + 0.5) * framebuffer->width;
        v0.y = (v0.y * 0.5 + 0.5) * framebuffer->height;
        v1.x = (v1.x * 0.5 + 0.5) * framebuffer->width;
        v1.y = (v1.y * 0.5 + 0.5) * framebuffer->height;
        v2.x = (v2.x * 0.5 + 0.5) * framebuffer->width;
        v2.y = (v2.y * 0.5 + 0.5) * framebuffer->height;

        // Dibujar los triángulos de la malla usando el algoritmo DDA
        framebuffer->DrawLineDDA(v0.x, v0.y, v1.x, v1.y, c);
        framebuffer->DrawLineDDA(v1.x, v1.y, v2.x, v2.y, c);
        framebuffer->DrawLineDDA(v2.x, v2.y, v0.x, v0.y, c);
    }
}

void Entity::Update(float seconds_elapsed) {
    // Definimos un ejemplo de transformación como puede ser rotar en el tiempo
    float angle = seconds_elapsed * 125.0f; // Rotamos 125 grados cada segundo
    modelMatrix.SetRotation(angle * DEG2RAD, Vector3(0, 1, 0)); // Rotamos alrededor del eje Y
}


