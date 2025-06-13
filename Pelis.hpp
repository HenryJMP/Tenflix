#pragma once // Previene inclusiones múltiples del archivo
#include "Videos.hpp" // Incluye la definición de la clase base Videos, de la cual hereda Pelis

// Definición de la clase Pelis que hereda públicamente de Videos
class Pelis : public Videos {
    public:
        // Constructor con todos los parámetros necesarios para inicializar una película
        Pelis(uint32_t, std::string, int, int, int, std::string, std::vector<double>, int, std::string);
        // Destructor virtual
        virtual ~Pelis() = default;
        // Métodos de acceso
        int getIdPeli() const; // Devuelve el ID de la película
        std::vector<double> getCalificaciones() const override; // Devuelve el vector de calificaciones
        // Métodos de modificación
        void setCalificaciones(double) override; // Agrega una nueva calificación
        //Extras
        void mostrarInformacion() const override; // Muestra la información completa de la película

    private:
        int id_peli; // ID único para identificar la película dentro del sistema
    
};
