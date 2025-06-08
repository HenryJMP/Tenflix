#include "Videos.hpp"

// Constructor
Videos::Videos(int _id, std::string _nombre, int _horas, int _minutos, int _segundos, std::string _genero, std::vector<float> _calificaciones, std::string _descripcion)
    : id{_id}, nombre{_nombre}, horas{_horas}, minutos{_minutos}, segundos{_segundos}, genero{_genero}, calificaciones{_calificaciones}, descripcion{_descripcion} {}

// Métodos de acceso
int Videos::getId() const {
    return id;
}

std::string Videos::getNombre() const {
    return nombre;
}

int Videos::getHoras() const {
    return horas;
}

int Videos::getMinutos() const {
    return minutos;
}

int Videos::getSegundos() const {
    return segundos;
}

std::string Videos::getGenero() const {
    return genero;
}

std::vector<float> Videos::getCalificaciones() const {
    return calificaciones;
}




