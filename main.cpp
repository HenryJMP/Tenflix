#include <iostream>
#include <jsoncpp/json/value.h> //ignorar error: es porque no está instalada en windows pero si en el entorno de Ubuntu, si compila
#include <jsoncpp/json/json.h> //ignorar error: es porque no está instalada en windows pero si en el entorno de Ubuntu, si compila
#include <fstream>
#include "Videos.hpp"
#include "Pelis.hpp"
#include "Serie.hpp"
#include "Cap.hpp"
#include <stdexcept>
#include <iomanip>

void CrearObjetos(const std::vector<std::string>& claves, const Json::Value& actualJson, 
                  std::vector<Pelis>& peliculas, std::vector<Serie>& series) {
    std::vector<uint32_t> ids_hex;
    for (const auto& clave : claves) {
        try {
            std::string nombre, genero, descripcion;
            std::vector <double> calificaciones;
            int horas, minutos, segundos, id_peli, id_serie, id_temp, id_cap;

            uint32_t id = std::stoul(clave, nullptr, 16);
            ids_hex.push_back(id);

            if (((id >> 16) & 0xFF) == 255 && ((id >> 8) & 0xFF) == 0) {
                id_peli = (id & 0xFF);
                nombre = actualJson[clave]["titulo"].asString();
                genero = actualJson[clave]["genero"].asString();
                horas = actualJson[clave]["duracion"]["horas"].asInt();
                minutos = actualJson[clave]["duracion"]["minutos"].asInt();
                segundos = actualJson[clave]["duracion"]["segundos"].asInt();
                descripcion = actualJson[clave]["descripcion"].asString();
                for (const auto& calif : actualJson[clave]["calificaciones"]) {
                    calificaciones.push_back(calif.asDouble());
                }
                std::vector<double> Calificaciones = calificaciones;
                Pelis pelicula(id, nombre, horas, minutos, segundos, genero, calificaciones, id_peli, descripcion);
                peliculas.push_back(pelicula);

            } else if (((id >> 16) & 0xFF) > 0  && ((id >> 8) & 0xFF) > 0 && (id & 0xFF) > 0){
                id_serie = (id >> 16) & 0xFF;
                id_temp = (id >> 8) & 0xFF;
                id_cap = id & 0xFF;
                nombre = actualJson[clave]["titulo"].asString();
                genero = "";
                horas = actualJson[clave]["duracion"]["horas"].asInt();
                minutos = actualJson[clave]["duracion"]["minutos"].asInt();
                segundos = actualJson[clave]["duracion"]["segundos"].asInt();
                descripcion = actualJson[clave]["descripcion"].asString();
                for (const auto& calif : actualJson[clave]["calificaciones"]) {
                    calificaciones.push_back(calif.asDouble());
                }
                std::vector<double> Calificaciones = calificaciones;
                Cap capitulo(id, nombre, horas, minutos, segundos, genero, calificaciones, descripcion, id_serie, id_temp, id_cap);
                if (series.empty() || series.back().getIdSerie() != id_serie) {
                    std::vector<Cap> capitulos;
                    capitulos.push_back(capitulo);
                    Serie serie(id, nombre, id_serie, genero, descripcion, calificaciones, capitulos);
                    series.push_back(serie);
                } else {
                    series.back().capitulos.push_back(capitulo);
                }

            } else if (((id >> 16) & 0xFF) > 0  && ((id >> 8) & 0xFF) == 0 && (id & 0xFF) == 0) {
                id_serie = (id >> 16) & 0xFF;
                nombre = actualJson[clave]["titulo"].asString();
                genero = actualJson[clave]["genero"].asString();
                descripcion = actualJson[clave]["descripcion"].asString();
                for (const auto& calif : actualJson[clave]["calificaciones"]) {
                    calificaciones.push_back(calif.asDouble());
                }
                std::vector<double> Calificaciones = calificaciones;
                Serie serie(id, nombre, id_serie, genero, descripcion, calificaciones, {});
                series.push_back(serie);
            } else {
                std::cerr << "ID no reconocido: 0x" << std::hex << id << std::dec << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error al convertir clave " << clave << ": " << e.what() << std::endl;
        }
    }
}

int main() {
    std::vector<Pelis> peliculas;
    std::vector<Serie> series; 

    std::ifstream archivo("Contenido.json");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo" << std::endl;
        return 1;
    }

    Json::Value actualJson;
    Json::CharReaderBuilder builder;
    std::string errores;
    if (!Json::parseFromStream(builder, archivo, &actualJson, &errores)) {
        std::cerr << "Error al parsear JSON: " << errores << std::endl;
        return 1;
    }

    std::vector<std::string> claves = actualJson.getMemberNames();
    CrearObjetos(claves, actualJson, peliculas, series);

    // Mostrar información de las películas
    std::cout << "Películas:" << std::endl;
    for (const auto& pelicula : peliculas) {
        pelicula.mostrarInformacion();
    }
    // Mostrar información de las series
    std::cout << "Series:" << std::endl;
    for (const auto& serie : series) {
        serie.mostrarInformacion();
        for (const auto& capitulo : serie.capitulos) {
            capitulo.mostrarInformacion();
        }
    }
    
    

    return 0;
}
