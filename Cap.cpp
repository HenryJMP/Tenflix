#include "Cap.hpp"

// Constructor
Cap::Cap(uint32_t _id, std::string _nombre, int _horas, int _minutos, int _segundos, std::string _genero, std::vector<double> _calificaciones, std::string _descripcion, int _id_serie, int _id_temp, int _id_cap)
    : Videos(_id, _nombre, _horas, _minutos, _segundos, _genero, _calificaciones, _descripcion), id_serie{_id_serie}, id_temp{_id_temp}, id_cap{_id_cap} {}

// Métodos de acceso
std::vector<double> Cap::getCalificaciones() const {
    return calificaciones;
}

int Cap::getIdSerie() const {
    return id_serie;
}

int Cap::getIdTemp() const {
    return id_temp;
}

int Cap::getIdCap() const {
    return id_cap;
}

// Métodos de modificación
void Cap::setCalificaciones() {
    // Implementación de la lógica para modificar las calificaciones
    // Por ejemplo, podrías agregar una nueva calificación al vector
    double nueva_calificacion;
    std::cout << "Ingrese una nueva calificación: ";
    std::cin >> nueva_calificacion;
    calificaciones.push_back(nueva_calificacion);
}

// Método para mostrar información
void Cap::mostrarInformacion() const {

    std::cout << "ID: " << std::hex << getId() << std::dec << std::endl;
    std::cout << "ID Serie: " << getIdSerie() << std::endl;
    std::cout << "ID Temporada: " << getIdTemp() << std::endl;
    std::cout << "ID Capítulo: " << getIdCap() << std::endl;
    std::cout << "Nombre: " << getNombre() << std::endl;
    std::cout << "Duración: " << getHoras() << " horas, " << getMinutos() << " minutos, " << getSegundos() << " segundos" << std::endl;
    std::cout << "Descripción: " << getDescripcion() << std::endl;

    if (!calificaciones.empty()) {
        double suma = 0.0;
        for (const auto& calificacion : calificaciones) {
            suma += calificacion;
        }
        double promedio = suma / calificaciones.size();
        std::cout << "Calificación promedio: " << std::fixed << std::setprecision(1) << promedio << std::endl;
    } else {
        std::cout << "No hay calificaciones disponibles." << std::endl;
    }
    std::cout << "----------------------------------------" << std::endl;
}


