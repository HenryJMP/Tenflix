#include "Videos.hpp"

//Constructor
Videos::Videos(std::string _id, std::string _nombre, std::string _genero, float _duracion): id{_id}, nombre{_nombre}, genero{_genero}, duracion{_duracion} {}

//Metodos de acceso
std::string Videos::getID() const{
    return id;
}

std::string Videos::getNombre() const{
    return nombre;
}

std::string Videos::getGenero() const{
    return genero;
}

float Videos::getDuracion() const{
    return duracion;
}


