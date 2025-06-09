#include "Serie.hpp"

// Constructor
Serie::Serie(uint32_t _id, std::string _nombre, int _id_serie, std::string _genero, std::string _descripcion, std::vector<double> _calificaciones, std::vector<Cap> _capitulos)
    : id(_id), nombre(std::move(_nombre)), id_serie(_id_serie), genero(std::move(_genero)), descripcion(std::move(_descripcion)), calificaciones(std::move(_calificaciones)), capitulos(std::move(_capitulos)) {}

// Métodos de acceso
uint32_t Serie::getId() const {
    return id;
}

std::string Serie::getNombre() const {
    return nombre;
}

int Serie::getIdSerie() const {
    return id_serie;
}

std::string Serie::getGenero() const {
    return genero;
}

std::string Serie::getDescripcion() const {
    return descripcion;
}

std::vector<double> Serie::getCalificaciones() {
    return calificaciones;
}

Cap Serie::getCapitulo(int index) const {
    if (index < 0 || index >= static_cast<int>(capitulos.size())) {
        throw std::out_of_range("Índice de capítulo fuera de rango");
    }
    return capitulos[index];
}

// Método para mostrar información
void Serie::mostrarInformacion() const {
    std::cout << "ID: " << std::hex << id << std::dec << std::endl;
    std::cout << "Nombre: " << nombre << std::endl;
    std::cout << "ID Serie: " << id_serie << std::endl;
    std::cout << "Género: " << genero << std::endl;
    std::cout << "Descripción: " << descripcion << std::endl;
    std::cout << "Calificacion: ";
    double suma = 0.0;
    for (const auto& calificacion : calificaciones) {
        suma += calificacion;
    }
    double promedio = calificaciones.empty() ? 0.0 : suma / calificaciones.size();
    std::cout << promedio << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}








