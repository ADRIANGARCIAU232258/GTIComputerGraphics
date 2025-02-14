
#include "entity.h"
#include "image.h"
#include "camera.h"

// Inicializamos el constructor de la clase Entity
Entity::Entity() : mesh(nullptr), modelMatrix(Matrix44()) {}

// Aqu� inicializanos el constructor de la clase Entity con par�metros de malla y matriz de modelo
Entity::Entity(Mesh* mesh, Matrix44 modelMatrix) : mesh(mesh), modelMatrix(modelMatrix) {}

// M�todo para cambiar el modo de renderizado
void Entity::SetRenderMode(eRenderMode newMode) {
    mode = newMode;
}

// Establecemos la malla de la entidad
void Entity::setMesh(Mesh* newMesh) {
    mesh = newMesh;
}

// Lo mismo para la matriz de modelo de la entidad
void Entity::setModelMatrix(Matrix44 newModelMatrix) {
    modelMatrix = newModelMatrix;
}

// Renderizamos la entidad en el framebuffer utilizando la c�mara y el color especificado
void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer) {
    const Color currentcolor;
    // Verificamos si la malla, el framebuffer o la c�mara no est�n definidos
    if (!mesh || !framebuffer || !camera) {
        std::cout << "Error: mesh, framebuffer o c�mara no est�n definidos" << std::endl; // Mensaje de depuraci�n utilizado en el desarrollo de la pr�ctica
        return;
    }

    // Obtenemos los v�rtices de la malla
    const std::vector<Vector3>& vertices = mesh->GetVertices();
	const std::vector<Vector2>& uvs = mesh->GetUVs();
    // Verifican si no hay v�rtices en la malla
    if (vertices.empty()) {
        std::cout << "Error: No hay v�rtices en la malla" << std::endl; // Lo mismo con este mensaje. Se ha usado con el fin de encontrar los errores surgidos
        return;
    }

    // Bucle for donde itermaos sobre los v�rtices de la malla de tres en tres
    for (size_t i = 0; i < vertices.size(); i += 3) {
        Vector3 v0 = vertices[i];
        Vector3 v1 = vertices[i + 1];
        Vector3 v2 = vertices[i + 2];

        // Aplicamos la matriz de modelo a cada uno de los v�rtices
        v0 = modelMatrix * v0;
        v1 = modelMatrix * v1;
        v2 = modelMatrix * v2;

        bool negZ0 = false, negZ1 = false, negZ2 = false;
        // Proyectamos los v�rtices usando la c�mara
        v0 = camera->ProjectVector(v0, negZ0);
        v1 = camera->ProjectVector(v1, negZ1);
        v2 = camera->ProjectVector(v2, negZ2);

        // Verificamos si los v�rtices que proyectamos est�n o no dentro del rango visible
        if (negZ0 || negZ1 || negZ2 || abs(v0.x) > 1 || abs(v0.y) > 1 || abs(v0.z) > 1 ||
            abs(v1.x) > 1 || abs(v1.y) > 1 || abs(v1.z) > 1 || abs(v2.x) > 1 || abs(v2.y) > 1 || abs(v2.z) > 1) {
            continue;
        }

        // En este fragmento convertimos las coordenadas de los v�rtices a coordenadas de pantalla
        v0.x = (v0.x * 0.5 + 0.5) * framebuffer->width;
        v0.y = (v0.y * 0.5 + 0.5) * framebuffer->height;
        v1.x = (v1.x * 0.5 + 0.5) * framebuffer->width;
        v1.y = (v1.y * 0.5 + 0.5) * framebuffer->height;
        v2.x = (v2.x * 0.5 + 0.5) * framebuffer->width;
        v2.y = (v2.y * 0.5 + 0.5) * framebuffer->height;

        // Creamos la estructura sTriangleInfo
        Image::sTriangleInfo triangle;
        triangle.vertices[0] = v0;
        triangle.vertices[1] = v1;
        triangle.vertices[2] = v2;
        triangle.colors[0] = Color(255, 0, 0);
        triangle.colors[1] = Color(0, 255, 0);
        triangle.colors[2] = Color(0, 0, 255);
        triangle.uvs[0] = uvs[i];
        triangle.uvs[1] = uvs[i + 1];
        triangle.uvs[2] = uvs[i + 2];
        triangle.texture = nullptr; // Asumimos que no hay textura por el momneto
        
        switch (mode) {
        case eRenderMode::POINTCLOUD:
            // Dibujar puntos (no implementado aqu�)
            break;

        case eRenderMode::WIREFRAME:
            // Usamos DrawLineDDA para dibujar las l�neas de los tri�ngulos
            framebuffer->DrawLineDDA((int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y, Color::GREEN);
            framebuffer->DrawLineDDA((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, Color::WHITE);
            framebuffer->DrawLineDDA((int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y, Color::BLUE);
            break;

        case eRenderMode::TRIANGLES:
            // Rellenamos tri�ngulos con color
            framebuffer->DrawTriangle(triangle, true);
            break;

        case eRenderMode::TRIANGLES_INTERPOLATED:
            // Rellenamos tri�ngulos con interpolaci�n de colores
            framebuffer->DrawTriangleInterpolated(triangle, zBuffer);
            break;
        }
    }
}

// Actualizamos la entidad con una rotaci�n en el tiempo
void Entity::Update(float seconds_elapsed) {
    // Definimos un ejemplo de transformaci�n como puede ser rotar en el tiempo
    float angle = seconds_elapsed * 125.0f; // Rotaci�on de  125 grados cada segundo
    Matrix44 rotationMatrix;
    rotationMatrix.SetRotation(angle * DEG2RAD, Vector3(0, 1, 0));
    modelMatrix = rotationMatrix * modelMatrix; // Aplicamos la rotaci�n al modelo
}



