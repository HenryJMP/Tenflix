En nuestro proyecto usamos un archivo JSON más que nada para tener la flexibilidad de cambiar el contenido sin cambiar el código.  Esto para guardar las películas, series y capítulos, cada uno con un ID único de 6 dígitos en hexadecimal. Ese ID lo dividimos en tres partes usando operaciones con bits: los primeros 2 dígitos (bits 16–23) indican la serie, los siguientes la temporada, y los últimos el número de capítulo o el ID de película.
————————-
La clase Serie, definida en Serie.h y Serie.cpp, representa una serie completa. Guarda su título, género, descripción y una lista de capítulos (Cap). Su función principal es organizar los capítulos que le pertenecen, calcular el promedio de calificaciones de todos ellos, y mostrar la información completa de la serie. Esto facilita tener todo agrupado y poder acceder a los capítulos fácilmente desde la interfaz.

Métodos clave:

* `agregarCapitulo(Capitulo* c)` → añade un capítulo a la serie.
* `mostrar()` → muestra la lista de capítulos con calificaciones.
* `getCapitulos()` → retorna la lista de capítulos.
* `getNombre(), getID()` → getters de serie que llaman al nombre y el ID

———————————-

La clase Pelis, definida en Pelis.h y Pelis.cpp, representa una película individual. Hereda de la clase base Video, por lo que reutiliza sus atributos y métodos. Solo se agrega el identificador único de la película (id_peli). Esta clase permite mostrar la información específica de una película y manejar sus calificaciones de forma independiente.

Método clave: mostrar()` → muestra los datos específicos de la película.
————
La clase Cap representa un capítulo dentro de una serie. Hereda de Videos, así que ya tiene atributos como nombre, duración, género, descripción, etc. Además, agrega tres datos específicos:
* idSerie: identifica a qué serie pertenece.
* idTemp: temporada del capítulo.
* idCap: número de capítulo.

*  Métodos clave de Cap:
* mostrarInformacion(): sobreescribe el de Videos para mostrar también temporada, número de capítulo y a qué serie pertenece.
* Getters como getIdSerie(), getIdTemp() y getIdCap(): permiten acceder a esos datos para ubicarlos correctamente.
* También tiene métodos para obtener el promedio de calificaciones, heredados o extendidos.
