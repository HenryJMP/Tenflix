#include "Videos.hpp"

//Constructor
Videos::Videos(std::string _titulo, std::string _genero, std::string _sinopsis, float _duracion, float _calificacion): titulo{_titulo}, genero{_genero}, sinopsis{_sinopsis}, duracion{_duracion}, calificacion{_calificacion}{}

//Metodos de acceso
std::string Videos::getTitulo() const{
    return titulo;
}

std::string Videos::getGenero() const{
    return genero;
}

std::string Videos::getSinopsis() const{
    return sinopsis;
}

float Videos::getDuracion() const{
    return calificacion;
}

float Videos::getCalificacion() const{
    return calificacion;
}


