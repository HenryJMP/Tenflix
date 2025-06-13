#include "Cap.hpp" // Incluye la definición de la clase Cap que hereda de Videos

// Constructor de Cap, llama al constructor de la clase base Videos e inicializa los atributos especifícos de Cap
Cap::Cap(uint32_t _id, std::string _nombre, int _horas, int _minutos, int _segundos, std::string _genero, std::vector<double> _calificaciones, std::string _descripcion, int _id_serie, int _id_temp, int _id_cap)
    : Videos(_id, _nombre, _horas, _minutos, _segundos, _genero, _calificaciones, _descripcion), id_serie{_id_serie}, id_temp{_id_temp}, id_cap{_id_cap} {}

// Métodos de acceso

// Devuelve el vector calificaciones
std::vector<double> Cap::getCalificaciones() const {
    return calificaciones;
}
// Devuelve el ID de la serie a la que pertenece este capítulo
int Cap::getIdSerie() const {
    return id_serie;
}
// Devuelve el número de temporada que pertenece este capítulo
int Cap::getIdTemp() const {
    return id_temp;
}
// Devuelve el número de capítulo dentro de la temporada
int Cap::getIdCap() const {
    return id_cap;
}

// Sobrecarga del operador de suma
Cap& Cap::operator+=(double calificacion) {
    calificaciones.push_back(calificacion);
    return *this;
}

// Permite al usuario ingresar una nueva calificación desde la consola y la agrega al vector de calificaciones
void Cap::setCalificaciones() {
    double nueva_calificacion;
    std::cout << "Ingrese una nueva calificación: ";
    std::cin >> nueva_calificacion;
    calificaciones.push_back(nueva_calificacion);
}

// Método para mostrar información de los capítulos en la consola
void Cap::mostrarInformacion() const {

    // Muestra el ID en formato hexadecimal
    std::cout << "ID: " << std::hex << getId() << std::dec << std::endl;

    // Muestra los IDs relacionados con la serie, temporada y capítulo
    std::cout << "ID Serie: " << getIdSerie() << std::endl;
    std::cout << "ID Temporada: " << getIdTemp() << std::endl;
    std::cout << "ID Capítulo: " << getIdCap() << std::endl;

    // Muestra el nombre del capítulo
    std::cout << "Nombre: " << getNombre() << std::endl;

    // Muestra la duración del capítulo en horas, minutos y segundos
    std::cout << "Duración: " << getHoras() << " horas, " << getMinutos() << " minutos, " << getSegundos() << " segundos" << std::endl;
    
    // Muestra la descripción del capítulo
    std::cout << "Descripción: " << getDescripcion() << std::endl;

    // Si hay calificaciones, se calcula y muestra el promedio
    if (!calificaciones.empty()) {
        double suma = 0.0;
        for (const auto& calificacion : calificaciones) {
            suma += calificacion;
        }
        double promedio = suma / calificaciones.size();
        std::cout << "Calificación promedio: " << std::fixed << std::setprecision(1) << promedio << std::endl;
    } else {
        std::cout << "No hay calificaciones disponibles." << std::endl; // Si no hay calificación se indica
    }
    std::cout << "----------------------------------------" << std::endl;
}


