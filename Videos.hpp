#pragma once // Evita que este archivo de encabezado sea incluido más de una vez
#include <iostream>
#include <vector>
#include <cstdint> // Para tipos de datos como unit32_t
#include <iomanip>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
// Serie 0 Temp 0 Cap 11? Es una pélicula
// id peli= 0x00000B
// Serue 10 Temo 2 Cap 11? Es el cap de una serie
// id cap=  0x0A020B
class Videos{
    public:
        // Constructor 
        Videos(uint32_t, std::string, int, int, int, std::string, std::vector<double>, std::string);
        // Destructor
        virtual ~Videos() = default;
        // Métodos de acceso 
        uint32_t getId() const;
        std::string getNombre() const;
        int getHoras() const;
        int getMinutos() const;
        int getSegundos() const;
        std::string getGenero() const;
        virtual std::vector<double> getCalificaciones() const = 0;
        std::string getDescripcion() const;
        // Métodos de modificación
        virtual void setCalificaciones(double) = 0; // Agrega una nueva calificación al vector de 
        // Método para mostrar información
        virtual void mostrarInformacion() const = 0;
        //Sobrecarga del operador de suma
        virtual Videos& operator+=(double calificacion);
   
    protected:
        std::vector<double> calificaciones;

    private:
        uint32_t id;
        std::string nombre;
        int horas;
        int minutos;
        int segundos;
        std::string genero;
        std::string descripcion;

};