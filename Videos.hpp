#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
// Serie 0 Temp 0 Cap 11? Es una pélicula
// id peli= 0x00000B
// Serue 10 Temo 2 Cap 11? Es el cap de una serie
// id cap=  0x0A020B

class Videos {
    public:
        // Constructor 
        Videos(int, std::string, int, int, int, std::string, std::vector<float>);
        // Constructor por defecto
        // Virtual destructor
        virtual ~Videos() = default;
        // Métodos de acceso
        int getId() const;
        std::string getNombre() const;
        int getHoras() const;
        int getMinutos() const;
        int getSegundos() const;
        std::string getGenero() const;
        std::vector<float> getCalificaciones() const;
        // Métodos de modificación
        virtual void setCalificacion(float);
        virtual void setCalificaciones(std::vector<float>);

    private:
        uint32_t id;
        std::string nombre;
        int horas;
        int minutos;
        int segundos;
        std::string genero;
        std::vector<float> calificaciones;
};
