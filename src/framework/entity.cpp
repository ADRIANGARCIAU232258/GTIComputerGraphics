
#include "entity.h"
#include "image.h"
#include "camera.h"

// Inicializamos el constructor de la clase Entity
Entity::Entity() : mesh(nullptr), modelMatrix(Matrix44()) {}

// Aquí inicializanos el constructor de la clase Entity con parámetros de malla y matriz de modelo
Entity::Entity(Mesh* mesh, Matrix44 modelMatrix) : mesh(mesh), modelMatrix(modelMatrix) {}

// Establecemos la malla de la entidad
void Entity::setMesh(Mesh* newMesh) {
    mesh = newMesh;
}

// Lo mismo para la matriz de modelo de la entidad
void Entity::setModelMatrix(Matrix44 newModelMatrix) {
    modelMatrix = newModelMatrix;
}

// Renderizamos la entidad en el framebuffer utilizando la cámara y el color especificado
void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
    // Verificamos si la malla, el framebuffer o la cámara no están definidos
    if (!mesh || !framebuffer || !camera) {
        std::cout << "Error: mesh, framebuffer o cámara no están definidos" << std::endl; // Mensaje de depuración utilizado en el desarrollo de la práctica
        return;
    }

    // Obtenemos los vértices de la malla
    const std::vector<Vector3>& vertices = mesh->GetVertices();
    // Verifican si no hay vértices en la malla
    if (vertices.empty()) {
        std::cout << "Error: No hay vértices en la malla" << std::endl; // Lo mismo con este mensaje. Se ha usado con el fin de encontrar los errores surgidos
        return;
    }

    // Bucle for donde itermaos sobre los vértices de la malla de tres en tres
    for (size_t i = 0; i < vertices.size(); i += 3) {
        Vector3 v0 = vertices[i];
        Vector3 v1 = vertices[i + 1];
        Vector3 v2 = vertices[i + 2];

        // Aplicamos la matriz de modelo a cada uno de los vértices
        v0 = modelMatrix * v0;
        v1 = modelMatrix * v1;
        v2 = modelMatrix * v2;

        bool negZ0 = false, negZ1 = false, negZ2 = false;
        // Proyectamos los vértices usando la cámara
        v0 = camera->ProjectVector(v0, negZ0);
        v1 = camera->ProjectVector(v1, negZ1);
        v2 = camera->ProjectVector(v2, negZ2);

        // Verificamos si los vértices que proyectamos están o no dentro del rango visible
        if (negZ0 || negZ1 || negZ2 || abs(v0.x) > 1 || abs(v0.y) > 1 || abs(v0.z) > 1 ||
            abs(v1.x) > 1 || abs(v1.y) > 1 || abs(v1.z) > 1 || abs(v2.x) > 1 || abs(v2.y) > 1 || abs(v2.z) > 1) {
            continue;
        }

        // En este fragmento convertimos las coordenadas de los vértices a coordenadas de pantalla
        v0.x = (v0.x * 0.5 + 0.5) * framebuffer->width;
        v0.y = (v0.y * 0.5 + 0.5) * framebuffer->height;
        v1.x = (v1.x * 0.5 + 0.5) * framebuffer->width;
        v1.y = (v1.y * 0.5 + 0.5) * framebuffer->height;
        v2.x = (v2.x * 0.5 + 0.5) * framebuffer->width;
        v2.y = (v2.y * 0.5 + 0.5) * framebuffer->height;

        // Dibujamos líneas entre los vértices proyectados para formar un triángulo
        framebuffer->DrawLineDDA(v0.x, v0.y, v1.x, v1.y, c);
        framebuffer->DrawLineDDA(v1.x, v1.y, v2.x, v2.y, c);
        framebuffer->DrawLineDDA(v2.x, v2.y, v0.x, v0.y, c);
    }
}

// Actualizamos la entidad con una rotación en el tiempo
void Entity::Update(float seconds_elapsed) {
    // Definimos un ejemplo de transformación como puede ser rotar en el tiempo
    float angle = seconds_elapsed * 125.0f; // Rotaciçon de  125 grados cada segundo
    Matrix44 rotationMatrix;
    rotationMatrix.SetRotation(angle * DEG2RAD, Vector3(0, 1, 0));
    modelMatrix = rotationMatrix * modelMatrix; // Aplicamos la rotación al modelo
}



