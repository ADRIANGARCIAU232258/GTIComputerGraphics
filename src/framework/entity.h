
#include "mesh.h"
#include "camera.h"
#include "image.h"

class Entity {
public:
    Mesh* mesh;
    Matrix44 modelMatrix;

    // Definimos los dos posibles tipos de constructores
    Entity();
    Entity(Mesh* mesh, Matrix44 modelMatrix);

    // Aquí definimos los setters para el mesh y la modelmatrix
    void setMesh(Mesh* newMesh);
    void setModelMatrix(Matrix44 newModelMatrix);

    // Método render para rasterizar las mallas de la entidad
    void Entity::Render(Image* framebuffer, Camera* camera, const Color& c);

    // Método para actualizar matrices modelo y adí poder rotarlas, etc...
    void Update(float seconds_elapsed);
};

