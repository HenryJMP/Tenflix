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

// Sobrecarga del operador de suma
Serie& Serie::operator+=(double calificacion) {
    calificaciones.push_back(calificacion);
    return *this;
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
    std::vector<double> promediosCapitulos;
    for (const auto& capitulo : capitulos) {
        const auto& califs = capitulo.getCalificaciones();
        double sumaCap = 0.0;
        for (double calif : califs) {
            sumaCap += calif;
        }
        double promedioCap = califs.empty() ? 0.0 : sumaCap / califs.size();
        promediosCapitulos.push_back(promedioCap);
    }
    double sumaPromedios = 0.0;
    for (double prom : promediosCapitulos) {
        sumaPromedios += prom;
    }
    double promedioSerie = promediosCapitulos.empty() ? 0.0 : sumaPromedios / promediosCapitulos.size();
    std::cout << promedioSerie << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}










