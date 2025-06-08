#include "Videos.hpp"

// Constructor
Videos::Videos(int id, std::string nombre, int horas, int minutos, int segundos, std::string genero, std::vector<float> calificaciones)
    : id(id), nombre(nombre), horas(horas), minutos(minutos), segundos(segundos), genero(genero), calificaciones(calificaciones) {}

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

// Métodos de modificación
void Videos::setCalificacion(float calificacion) {
    calificaciones.push_back(calificacion);
}

void Videos::setCalificaciones(std::vector<float> nuevasCalificaciones) {
    calificaciones = nuevasCalificaciones;
}


