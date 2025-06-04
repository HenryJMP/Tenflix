#pragma once
#include <iostream>

class Videos{
    public:
        //Constructor 
        Videos(std::string, std::string, std::string, float, float);
        //Metodos de acceso
        std::string getTitulo() const;
        std::string getGenero() const;
        std::string getSinopsis() const;
        float getDuracion() const;
        float getCalificacion() const;
        //Metodos modificadores

        //Extras
        virtual float calificacion() const = 0;

    protected:
    
    private:
        std::string titulo;
        std::string genero;
        std::string sinopsis;
        float duracion;
        float calificacion;

};