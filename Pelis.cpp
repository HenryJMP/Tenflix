#include "Pelis.hpp" // Incluye el archivo de cabecera de la clase Pelis, que hereda de la clase videos

// Constructor de la clase Pelis, llama al constructor de la clase base Videos e inicializa el atributo específico id_peli 
Pelis::Pelis(uint32_t _id, std::string _nombre, int _horas, int _minutos, int _segundos, std::string _genero, std::vector<double> _calificaciones, int _id_peli, std::string _descripcion)
    : Videos(_id, _nombre, _horas, _minutos, _segundos, _genero, _calificaciones, _descripcion), id_peli{_id_peli} {}

// Métodos de acceso
int Pelis::getIdPeli() const { // Devuelve el ID de la película
    return id_peli;
}
// Devuelve el vector de calificaciones (una copia)
std::vector<double> Pelis::getCalificaciones() const {
    return calificaciones;
}

// Métodos de modificación
void Pelis::setCalificaciones(double nueva_calificacion) { // Agrega una nueva calificación al vector
    calificaciones.push_back(nueva_calificacion);
}

// Método para mostrar información de la Película en consola
void Pelis::mostrarInformacion() const {
    std::cout << "ID: " << std::hex << Videos::getId() << std::dec << std::endl; // Muestra el ID de la película en formato hexadecimal
    
    // Muestra el ID de película, nombre y duración
    std::cout << "Pelicula ID: " << getIdPeli() << std::endl;
    std::cout << "Nombre: " << getNombre() << std::endl;
    std::cout << "Duración: " << getHoras() << " horas, " << getMinutos() << " minutos, " << getSegundos() << " segundos" << std::endl;
    std::cout << "Género: " << getGenero() << std::endl;
    std::cout << "Descripción: " << getDescripcion() << std::endl;
    
    // Si hay calificaciones, se calcula el promedio y se muestra
    if (!calificaciones.empty()) {
        double suma = 0.0;
        for (const auto& calificacion : calificaciones) {
            suma += calificacion;
        }
        double promedio = suma / calificaciones.size();
        std::cout << "Calificación promedio: " << std::fixed << std::setprecision(1) << promedio << std::endl;
    } else {
        std::cout << "No hay calificaciones disponibles." << std::endl; // Si no hay calificaciones se indica
    }
    std::cout << "----------------------------------------" << std::endl;
}
