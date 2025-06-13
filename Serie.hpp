#pragma once // Previene múltiples inclusiones de este encabezado durante la compilación
#include "Cap.hpp"  // Incluye la definición de la clase Cap porque una serie  contiene capítulos

class Serie {
    public:
        // Lista de capítulos que componen la serie
        std::vector<Cap> capitulos;
        // Constructor que inicializa todos los atributos de la clase
        Serie(uint32_t _id, std::string _nombre, int _id_serie, std::string _genero, std::string _descripcion, std::vector<double> _calificaciones, std::vector<Cap>);
        // Destructor virtual
        virtual ~Serie() = default;
        // Métodos de acceso
        uint32_t getId() const; // Devuelve el ID único de la serie en hexadecimal
        std::string getNombre() const; // Devuelve el nombre de la serie
        int getIdSerie() const; // Devuelve el identificador interno de la serie
        std::string getGenero() const; // Devuelve el género
        std::string getDescripcion() const; // Devuelve una descripción general de la serie
        std::vector<double> getCalificaciones(); // Devuelve el vector de calificaciones de la serie
        Cap getCapitulo(int) const; // Devuelve un capítulo específico según su índice. Lanza una excepción si el índice está fuera de rango
        // Método para mostrar información
        void mostrarInformacion() const; // Imprime en consola toda la información relevante de la serie. Incluye el promedio de calificaciones por capítulo

    protected:
        std::vector<double> calificaciones; // Las calificaciones de la serie en general. Protegido para que posibles subclases puedan acceder a ellas

    private:
        uint32_t id; // ID de la serie
        std::string nombre; // Nombre de la serie
        int id_serie; // ID interno de la serie
        std::string genero; // Género de la serie
        std::string descripcion; // Breve descripción del contenido de la serie

};
