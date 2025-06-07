#pragma once
#include <iostream>
#include <vector>

class Videos {
public:
    // Constructor 
    Videos(std::string, std::string, std::string, float);
    // Virtual destructor
    virtual ~Videos() = default;
    // Métodos de acceso
    std::string getID() const;
    std::string getNombre() const;
    std::string getGenero() const;
    float getDuracion() const;
    float getCalificacion() const;
    // Método para modificar la calificación
    virtual void setCalificacion(float nuevaCalificacion) = 0;

protected:
    std::string id;
    std::string nombre;
    std::string genero;
    float duracion;
    float calificacion;
};
