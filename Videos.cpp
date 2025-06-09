#include "Videos.hpp"

// Constructor
Videos::Videos(uint32_t _id, std::string _nombre, int _horas, int _minutos, int _segundos, std::string _genero, std::vector<double> _calificaciones, std::string _descripcion)
    : id{_id}, nombre{_nombre}, horas{_horas}, minutos{_minutos}, segundos{_segundos}, genero{_genero}, calificaciones{_calificaciones}, descripcion{_descripcion} {}

// Métodos de acceso
uint32_t Videos::getId() const {
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

std::string Videos::getDescripcion() const {
    return descripcion;
}

//Metodos de modificación
void Videos::setCalificaciones() {
    float nueva_calificacion;
    std::cout << "Ingrese una nueva calificación: ";
    std::cin >> nueva_calificacion;
    calificaciones.push_back(nueva_calificacion);
}
