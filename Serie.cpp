#include "Serie.hpp" // Incluye el archivo de cabecera de la clase Serie, donde se definen sus atributos y métodos

// Constructor que inicializa todos los atributos de Serie usando lista de inicialización (se usa std::move para evitar copias innecesarias de strings y vectores)
Serie::Serie(uint32_t _id, std::string _nombre, int _id_serie, std::string _genero, std::string _descripcion, std::vector<double> _calificaciones, std::vector<Cap> _capitulos)
    : id(_id), nombre(std::move(_nombre)), id_serie(_id_serie), genero(std::move(_genero)), descripcion(std::move(_descripcion)), calificaciones(std::move(_calificaciones)), capitulos(std::move(_capitulos)) {}

// Métodos de acceso
uint32_t Serie::getId() const {
    return id; // Devuelve el ID de la serie en hexadecimal
}

std::string Serie::getNombre() const {
    return nombre; // Devuelve el nombre de la serie
}

int Serie::getIdSerie() const {
    return id_serie; // Devuelve el identificador interno de la serie
}

std::string Serie::getGenero() const {
    return genero; // Devuelve el género
}

std::string Serie::getDescripcion() const {
    return descripcion; // Devuelve la descripción
}

std::vector<double> Serie::getCalificaciones() {
    return calificaciones; // Devuelve el vector de calificaciones
}
// Devuelve un capítulo específico, validando que el índice esté dentro del rango
Cap Serie::getCapitulo(int index) const {
    if (index < 0 || index >= static_cast<int>(capitulos.size())) {
        throw std::out_of_range("Índice de capítulo fuera de rango");
    }
    return capitulos[index];
}
// Método para mostrar información
void Serie::mostrarInformacion() const {
    // Muestra los datos generales de la serie
    std::cout << "ID: " << std::hex << id << std::dec << std::endl; // ID en hexadecimal
    std::cout << "Nombre: " << nombre << std::endl;
    std::cout << "ID Serie: " << id_serie << std::endl;
    std::cout << "Género: " << genero << std::endl;
    std::cout << "Descripción: " << descripcion << std::endl;
    std::cout << "Calificacion: "; // Calcular y mostrar la calificación promedio basada en los capítulos
    double suma = 0.0;
    std::vector<double> promediosCapitulos;
    // Recorre todos los capítulos y calcula el promedio de cada uno
    for (const auto& capitulo : capitulos) {
        const auto& califs = capitulo.getCalificaciones();
        double sumaCap = 0.0;
        for (double calif : califs) {
            sumaCap += calif;
        }
        double promedioCap = califs.empty() ? 0.0 : sumaCap / califs.size();
        promediosCapitulos.push_back(promedioCap); // Guarda el promedio del capítulo
    }
    // Calcula el promedio de todos los promedios de los capítulos
    double sumaPromedios = 0.0;
    for (double prom : promediosCapitulos) {
        sumaPromedios += prom;
    }
    double promedioSerie = promediosCapitulos.empty() ? 0.0 : sumaPromedios / promediosCapitulos.size();
    std::cout << promedioSerie << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}
