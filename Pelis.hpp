#pragma once
#include "Videos.hpp"

class Pelis : public Videos {
    public:
        // Constructor
        Pelis(uint32_t, std::string, int, int, int, std::string, std::vector<double>, int, std::string);
        // Destructor
        virtual ~Pelis() = default;
        // Métodos de acceso
        int getIdPeli() const;
        std::vector<double> getCalificaciones() const override;
        // Métodos de modificación
        void setCalificaciones() override;
        //Extras
        void mostrarInformacion() const override;

    private:
        int id_peli;
    
};
