#pragma once // Evita que este archivo de encabezado sea incluido más de una vez
#include <iostream> // Para std::cout y otros objetos de entrada/salida
#include <vector> // Para usar std::vector
#include <cstdint> // Para tipos de datos como unit32_t
#include <iomanip> // Para formateo de salida
#include <QApplication> // Parte de Qt, para aplicaciones gráficas
#include <QMainWindow> // Ventana principal en Qt
#include <QPushButton> // Botones en Qt
#include <QLabel> // Etiquetas de texto en Qt
#include <QVBoxLayout> // Diseño vertical en Qt
#include <QWidget> // Clase base de todos los elementos gráficos en Qt
// Serie 0 Temp 0 Cap 11? Es una pélicula
// id peli= 0x00000B
// Serue 10 Temo 2 Cap 11? Es el cap de una serie
// id cap=  0x0A020B

class Videos{
    public:
        // Constructor que recibe todos los datos necesarios
        Videos(uint32_t, std::string, int, int, int, std::string, std::vector<double>, std::string);
        // Destructor virtual
        virtual ~Videos() = default;
        // Métodos de acceso 
        uint32_t getId() const; // Devuelve el ID del video
        std::string getNombre() const; // Devuelve el nombre
        int getHoras() const; // Devuelve las horas de duración 
        int getMinutos() const; // Devuelve los minutos de duración
        int getSegundos() const; // Devuelve los segundos de duración
        std::string getGenero() const; // Devuelve el género
        virtual std::vector<double> getCalificaciones() const = 0; // Método puro virtual, obliga a las clases hijas a implementarlo, devuelve el vector de calificaciones
        std::string getDescripcion() const; // Devuelve la descripción
        // Métodos de modificación
        virtual void setCalificaciones(double); // Método virtual (se puede sobreescribir). Agrega una nueva calificación
        // Método para mostrar información
        virtual void mostrarInformacion() const = 0; // Método puro virtual: las clases hijas deben definir cómo mostrar la información 
   
    protected:
        std::vector<double> calificaciones; // Protegido: permite acceso desde las clases hijas como Cap o Pelis

    private:
        // Atributos privados, comunes a cualquier tipo de video
        uint32_t id; // ID único
        std::string nombre; // Nombre del video
        int horas; // Duración en horas
        int minutos; // Duración en minutos
        int segundos; // Duración en segundos
        std::string genero; // Género del video
        std::string descripcion; // Descripción del contenido

};
