#include "Serie.hpp"

// Constructor
Serie::Serie(uint32_t _id, std::string _nombre, int _horas, int _minutos, int _segundos, std::string _genero, std::vector<float> _calificaciones,
             int _id_serie, int _id_temp, int _temporada, std::vector<int> _capitulos): Videos(_id, _nombre, _horas, _minutos, _segundos, _genero, _calificaciones, "Descripción de la serie"),
                id_serie{_id_serie}, id_temp{_id_temp}, temporada{_temporada}, capitulos{_capitulos} {}

// Métodos de acceso
int Serie::getIdSerie() const {
    return id_serie;
}

int Serie::getIdTemp() const {
    return id_temp;
}

int Serie::getTemporada() const {
    return temporada;
}

std::vector<int> Serie::getCapitulos() const {
    return capitulos;
}

std::vector<float> Serie::getCalificaciones() const {
    return Videos::getCalificaciones();
}

void mostrarCapitulos(const std::vector<int>& capitulos) {
    std::cout << "Capítulos: ";
    for (const auto& cap : capitulos) {
        std::cout << cap << " ";
    }
    std::cout << std::endl;
}

void Serie::mostrarInformacion() const { 
    std::cout << "Serie ID: " << getIdSerie() << std::endl;
    std::cout << "Nombre: " << getNombre() << std::endl;
    std::cout << "Duración: " << getHoras() << " horas, " << getMinutos() << " minutos, " << getSegundos() << " segundos" << std::endl;
    std::cout << "Género: " << getGenero() << std::endl;
    mostrarCapitulos(capitulos);
    std::cout << "Calificaciones: ";
    for (const auto& calificacion : getCalificaciones()) {
        std::cout << calificacion << " ";
    }
    std::cout << std::endl;
    std::cout << "Descripción: " << getDescripcion() << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}




