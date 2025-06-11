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

void guardarJSON(const std::vector<Pelis>& peliculas, const std::vector<Serie>& series) {
    Json::Value root;
    for (const auto& pelicula : peliculas) {
        Json::Value peliJson;
        peliJson["titulo"] = pelicula.getNombre();
        peliJson["genero"] = pelicula.getGenero();
        peliJson["duracion"]["horas"] = pelicula.getHoras();
        peliJson["duracion"]["minutos"] = pelicula.getMinutos();
        peliJson["duracion"]["segundos"] = pelicula.getSegundos();
        peliJson["descripcion"] = pelicula.getDescripcion();
        for (const auto& calif : pelicula.getCalificaciones()) {
            peliJson["calificaciones"].append(calif);
        }
        // Guardar el id en formato hexadecimal con 0x al principio
        std::stringstream ss;
        ss << "0x" << std::hex << std::uppercase << pelicula.getId();
        root[ss.str()] = peliJson;
    }

    for (const auto& serie : series) {
        Json::Value serieJson;
        serieJson["titulo"] = serie.getNombre();
        serieJson["genero"] = serie.getGenero();
        serieJson["descripcion"] = serie.getDescripcion();
        for (const auto& capitulo : serie.capitulos) {
            Json::Value capJson;
            capJson["titulo"] = capitulo.getNombre();
            capJson["duracion"]["horas"] = capitulo.getHoras();
            capJson["duracion"]["minutos"] = capitulo.getMinutos();
            capJson["duracion"]["segundos"] = capitulo.getSegundos();
            capJson["descripcion"] = capitulo.getDescripcion();
            for (const auto& calif : capitulo.getCalificaciones()) {
                capJson["calificaciones"].append(calif);
            }
            serieJson["capitulos"].append(capJson);
        }
        std::stringstream ss;
        ss << "0x" << std::hex << std::uppercase << serie.getId();
        root[ss.str()] = serieJson;
    }

    std::ofstream archivo("Contenido.json");
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo para guardar" << std::endl;
        return;
    }
    archivo << root.toStyledString();
}

int main() {
    std::vector<Pelis> peliculas;
    std::vector<Serie> series;
    int opcion; 

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

    do {
        std::cout << "Seleccione una opción:" << std::endl;
        std::cout << "1. Mostrar películas" << std::endl;
        std::cout << "2. Mostrar series" << std::endl;
        std::cout << "3. Agregar calificación a una película o capítulo" << std::endl;
        std::cout << "4. Salir" << std::endl;
        std::cin >> opcion;

        switch (opcion) {
            case 1: {
                system("clear");
                for (const auto& pelicula : peliculas) {
                    pelicula.mostrarInformacion();
                }
                std::cin.ignore();
                std::cout << "Presiona enter para continuar...";
                std::cin.get();
                break;
            }
            case 2: {
                system("clear");
                for (const auto& serie : series) {
                    serie.mostrarInformacion();
                }
                std::cin.ignore();
                std::cout << "Presiona enter para continuar...";
                std::cin.get();
                break;
            }
            case 3: {
                system("clear");
                int id;
                std::cout << "Ingrese el ID de la película o capítulo (en hexadecimal): ";
                std::cin >> std::hex >> id;

                bool encontrado = false;
                for (auto& pelicula : peliculas) {
                    if (pelicula.getId() == id) {
                        pelicula.setCalificaciones();
                        encontrado = true;
                        break;
                    }
                }

                if (!encontrado) {
                    for (auto& serie : series) {
                        for (auto& capitulo : serie.capitulos) {
                            if (capitulo.getId() == id) {
                                capitulo.setCalificaciones();
                                encontrado = true;
                                break;
                            }
                        }
                        if (encontrado) break;
                    }
                }

                if (!encontrado) {
                    std::cout << "ID no encontrado." << std::endl;
                } else {
                    std::cout << "Calificación agregada correctamente." << std::endl;
                }
                std::cin.ignore();
                std::cout << "Presiona enter para continuar...";
                std::cin.get();
                break;
            }
            case 4:
                std::cout << "Saliendo del programa..." << std::endl;
                break;
            default:
                std::cout << "Opción inválida, intente de nuevo." << std::endl;
        }

    } while (opcion != 4);


    // Guardar el JSON actualizado
    guardarJSON(peliculas, series);
    std::cout << "Contenido guardado en Contenido.json" << std::endl;
    archivo.close();


    return 0;
}
