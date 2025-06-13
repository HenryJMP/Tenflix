#pragma once
#include "Cap.hpp"

class Serie {
    public:
        std::vector<Cap> capitulos;
        // Constructor
        Serie(uint32_t _id, std::string _nombre, int _id_serie, std::string _genero, std::string _descripcion, std::vector<double> _calificaciones, std::vector<Cap>);
        // Destructor
        virtual ~Serie() = default;
        // Métodos de acceso
        uint32_t getId() const;
        std::string getNombre() const;
        int getIdSerie() const;
        std::string getGenero() const;
        std::string getDescripcion() const;
        std::vector<double> getCalificaciones();
        Cap getCapitulo(int) const;
        // Método para mostrar información
        void mostrarInformacion() const;

    protected:
        std::vector<double> calificaciones;

    private:
        uint32_t id;
        std::string nombre;
        int id_serie;
        std::string genero;
        std::string descripcion;

};
