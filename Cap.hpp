#pragma once
#include "Videos.hpp"

class Cap : public Videos {
      public:
         // Constructor
         Cap(uint32_t, std::string, int, int, int, std::string, std::vector<double>, std::string, int, int, int);
         // Destructor
         virtual ~Cap() = default;
         // Métodos de acceso
         int getIdSerie() const;
         int getIdTemp() const;
         int getIdCap() const;
         std::vector<double> getCalificaciones() const override;
         // Métodos de modificación
         void setCalificaciones() override;
         // Método para mostrar información
         void mostrarInformacion() const override;
         // Sobrecarga del operador de suma
         Cap& operator+=(double calificacion) override;

      private:
         int id_serie;
         int id_temp;
         int id_cap;
      


};

