
#include "mesh.h"
#include "camera.h"
#include "image.h"

class Entity {
public:
    Mesh* mesh;
    Matrix44 modelMatrix;

    // Creamos una variable booleana que dir� si se debe interpolar o no los tri�ngulos
    bool toggleInterpolation = false;

    // Funci�n para hacer el cambio entre Interpalated UVs y Filled Triangles
    void InterpolationMode() { 
        toggleInterpolation = !toggleInterpolation;
        std::cout << "Modo de interpolaci�n cambiado: " << (toggleInterpolation ? "Interpolado" : "S�lido") << std::endl;
    };

    // Definimos los dos posibles tipos de constructores
    Entity();
    Entity(Mesh* mesh, Matrix44 modelMatrix);

    // Aqu� definimos los setters para el mesh y la modelmatrix
    void setMesh(Mesh* newMesh);
    void setModelMatrix(Matrix44 newModelMatrix);

    // M�todo render que utilizaremos para rasterizar las mallas de la entidad
    void Entity::Render(Image* framebuffer, Camera* camera, const Color& c);

    // M�todo usado para actualizar matrices modelo y ad� poder rotarlas, etc...
    void Update(float seconds_elapsed);
};

