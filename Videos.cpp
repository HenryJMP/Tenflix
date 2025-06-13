#include "Videos.hpp" // Incluye la declaración de la clase base Videos

// Constructor que inicializa los atributos de la clase Videos usando lista de inicialización 
Videos::Videos(uint32_t _id, std::string _nombre, int _horas, int _minutos, int _segundos, std::string _genero, std::vector<double> _calificaciones, std::string _descripcion)
    : id{_id}, nombre{_nombre}, horas{_horas}, minutos{_minutos}, segundos{_segundos}, genero{_genero}, calificaciones{_calificaciones}, descripcion{_descripcion} {}

// Métodos de acceso
uint32_t Videos::getId() const {
    return id; // Devuelve el ID del video
}

std::string Videos::getNombre() const {
    return nombre; // Devuelve el nombre del video
}

int Videos::getHoras() const {
    return horas; // Devuelve las horas de duración
}

int Videos::getMinutos() const {
    return minutos; // Devuelve los minutos de duración
}

int Videos::getSegundos() const {
    return segundos; // Devuelve los segundos de duración
}

std::string Videos::getGenero() const {
    return genero; // Devuelve el género del video
}

std::string Videos::getDescripcion() const {
    return descripcion; // Devuelve la descripción del video
}

//Metodos de modificación
void Videos::setCalificaciones(double nueva_calificacion) {
    calificaciones.push_back(nueva_calificacion); // Agrega una nueva calificación al vector de calificaciones
}
