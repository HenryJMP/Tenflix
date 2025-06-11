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
    std::ifstream archivo_lectura("Contenido.json");
    Json::Value root;
    if (archivo_lectura.is_open()) {
        Json::CharReaderBuilder builder;
        std::string errores;
        if (!Json::parseFromStream(builder, archivo_lectura, &root, &errores)) {
            std::cerr << "Error al parsear JSON para guardar: " << errores << std::endl;
            archivo_lectura.close();
            return;
        }
        archivo_lectura.close();
    } else {
        std::cerr << "Error al abrir Contenido.json para lectura" << std::endl;
        return;
    }

    for (const auto& pelicula : peliculas) {
        std::stringstream ss;
        ss << "0x" << std::hex << std::setw(6) << std::setfill('0') << std::uppercase << pelicula.getId();
        std::string clave = ss.str();
        if (root.isMember(clave)) {
            root[clave]["calificaciones"].clear();
            for (const auto& calif : pelicula.getCalificaciones()) {
                root[clave]["calificaciones"].append(calif);
            }
        }
    }

    for (const auto& serie : series) {
        for (const auto& capitulo : serie.capitulos) {
            std::stringstream ss;
            ss << "0x" << std::hex << std::setw(6) << std::setfill('0') << std::uppercase << capitulo.getId();
            std::string clave = ss.str();
            if (root.isMember(clave)) {
                root[clave]["calificaciones"].clear();
                for (const auto& calif : capitulo.getCalificaciones()) {
                    root[clave]["calificaciones"].append(calif);
                }
            }
        }
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
                if (series.empty()) {
                    std::cout << "No hay series disponibles." << std::endl;
                } else {
                    for (const auto& serie : series) {
                        serie.mostrarInformacion();
                    }
                    std::cout << "Seleccione el número de la serie para ver sus capítulos:" << std::endl;
                    for (size_t i = 0; i < series.size(); ++i) {
                        std::cout << i + 1 << ". " << series[i].getNombre() << std::endl;
                    }
                    size_t seleccion;
                    std::cout << "Ingrese el número de la serie: ";
                    std::cin >> seleccion;
                    if (seleccion > 0 && seleccion <= series.size()) {
                        system("clear");
                        std::cout << "Capítulos de la serie \"" << series[seleccion - 1].getNombre() << "\":" << std::endl;
                        const auto& capitulos = series[seleccion - 1].capitulos;
                        if (capitulos.empty()) {
                            std::cout << "No hay capítulos disponibles para esta serie." << std::endl;
                        } else {
                            for (const auto& capitulo : capitulos) {
                                capitulo.mostrarInformacion();
                            }
                        }
                    } else {
                        std::cout << "Selección inválida." << std::endl;
                    }
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
