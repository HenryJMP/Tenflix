#include "Pelis.hpp"

// Constructor
Pelis::Pelis(uint32_t _id, std::string _nombre, int _horas, int _minutos, int _segundos, std::string _genero, std::vector<double> _calificaciones, int _id_peli, std::string _descripcion)
    : Videos(_id, _nombre, _horas, _minutos, _segundos, _genero, _calificaciones, _descripcion), id_peli{_id_peli} {}

// Métodos de acceso
int Pelis::getIdPeli() const {
    return id_peli;
}

std::vector<double> Pelis::getCalificaciones() const {
    return calificaciones;
}

// Métodos de modificación
void Pelis::setCalificaciones() {
    // Implementación de la lógica para modificar las calificaciones
    // Por ejemplo, podrías agregar una nueva calificación al vector
    float nueva_calificacion;
    std::cout << "Ingrese una nueva calificación: ";
    std::cin >> nueva_calificacion;
    calificaciones.push_back(nueva_calificacion);
}

// Método para mostrar información
void Pelis::mostrarInformacion() const {
    std::cout << "ID: " << std::hex << Videos::getId() << std::dec << std::endl;
    std::cout << "Pelicula ID: " << getIdPeli() << std::endl;
    std::cout << "Nombre: " << getNombre() << std::endl;
    std::cout << "Duración: " << getHoras() << " horas, " << getMinutos() << " minutos, " << getSegundos() << " segundos" << std::endl;
    std::cout << "Género: " << getGenero() << std::endl;
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

