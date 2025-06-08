#pragma once
#include "Serie.hpp"

class Cap :   public Serie {
    public:
        // Constructor
        Cap(uint32_t, std::string, int, int, int, std::string, std::vector<float>, int, int, int, std::vector<int>, int);
        //Destructor
        virtual ~Cap() = default;
        // Métodos de acceso
        int getIdCapitulo() const;
        std::vector<float> getCalificaciones() const override;
        // Métodos de modificación
        void setCalificaciones() override;
        // Método para mostrar información
        void mostrarInformacion() const override;

    private:
        int id_capitulo; 
        
};

