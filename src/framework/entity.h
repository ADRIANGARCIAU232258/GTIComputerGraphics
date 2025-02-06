
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

    // Aqu� definimos los setters para el mesh y la modelmatrix
    void setMesh(Mesh* newMesh);
    void setModelMatrix(Matrix44 newModelMatrix);

    // M�todo render para rasterizar las mallas de la entidad
    void Entity::Render(Image* framebuffer, Camera* camera, const Color& c);

    // M�todo para actualizar matrices modelo y ad� poder rotarlas, etc...
    void Update(float seconds_elapsed);
};

