
#include "mesh.h"
#include "camera.h"
#include "image.h"

// A�adimos la enumeraci�n eRenderMode
enum class eRenderMode {
    POINTCLOUD,   
    WIREFRAME,        
    TRIANGLES,        
    TRIANGLES_INTERPOLATED 
};

class Entity {
public:
    Mesh* mesh;
    Matrix44 modelMatrix;

    // Agregamos miembro con el modo de renderizado actual
    eRenderMode mode = eRenderMode::TRIANGLES; // Establecemos el valor por defecto en TRIANGLES

    void SetRenderMode(eRenderMode newMode);  // M�todo para cambiar el modo de renderizado

    // Definimos los dos posibles tipos de constructores
    Entity();
    Entity(Mesh* mesh, Matrix44 modelMatrix);

    // Aqu� definimos los setters para el mesh y la modelmatrix
    void setMesh(Mesh* newMesh);
    void setModelMatrix(Matrix44 newModelMatrix);

    // M�todo render que utilizaremos para rasterizar las mallas de la entidad
    void Entity::Render(Image* framebuffer, Camera* camera, FloatImage* zBuffer);   

    // M�todo usado para actualizar matrices modelo y ad� poder rotarlas, etc...
    void Update(float seconds_elapsed);
};

