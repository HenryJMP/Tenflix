#pragma once
#include "Videos.hpp"

class Serie : public Videos {
    public:
        // Constructor
        Serie(uint32_t, std::string, int, int, int, std::string, std::vector<float>, int, int, int, std::vector<int>);
        // Métodos de acceso
        int getIdSerie() const;
        int getIdTemp() const;
        int getTemporada() const;
        std::vector<int> getCapitulos() const;
        std::vector<float> getCalificaciones() const override;
        // Métodos de modificación
        void setCalificaciones() override;
        //Extras
        void mostrarInformacion() const override;
    
    private:
        int id_serie;
        int id_temp;
        int temporada;
        std::vector<int> capitulos;


};