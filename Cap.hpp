#pragma once // Evita que el archivo se incluya más de una vez durante la compilación
#include "Videos.hpp" // Incluye la clase videos, de la cual hereda Cap
#include <QMetaType> 

// Definición de la clase Cap que hereda de Videos
class Cap : public Videos {
      public:
         // Constructor
         Cap(uint32_t, std::string, int, int, int, std::string, std::vector<double>, std::string, int, int, int);
         // Destructor virtual por si se utiliza polimorfismo; usa =default para indicar implementación por defecto
         virtual ~Cap() = default;
         // Métodos de acceso
         int getIdSerie() const; // Obtiene el ID de la serie
         int getIdTemp() const; // Obtiene el número de temporada
         int getIdCap() const; // Obtiene el número de capítulo
         std::vector<double> getCalificaciones() const override; // Obtiene las calificaciones
         // Métodos de modificación
         void setCalificaciones(double) override; // Agrega una nueva calificación
         // Método para mostrar todos los datos del capítulo
         void mostrarInformacion() const override; // Muestra la información del capítulo
         
      private:
         // Atributos específicos del capítulo
         int id_serie; // ID de la serie a la que pertenece este capítulo
         int id_temp; // Número de temporada
         int id_cap; // Número de capítulo
      

};
