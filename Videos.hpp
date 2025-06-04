#pragma once
#include <iostream>

class Videos{
    public:
        //Constructor 
        Videos(std::string, std::string, std::string, float);
        //Metodos de acceso
        std::string getID() const;
        std::string getNombre() const;
        std::string getGenero() const;
        float getDuracion() const;
        //Metodos modificadores

        //Extras
        virtual float calificacion() const = 0;

    protected:
    
    private:
        std::string id;
        std::string nombre;
        std::string genero;
        float duracion;

};