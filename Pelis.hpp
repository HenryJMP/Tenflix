#include "Videos.hpp"

class Pelis : public Videos {
    public:
        // Constructor
        Pelis(int, std::string, int, int, int, std::string, std::vector<float>, int, std::string);
        // Destructor
        virtual ~Pelis() = default;
        // Métodos de acceso
        int getIdPeli() const;
        std::vector<float> getCalificaciones() const override;
        // Métodos de modificación
        void setCalificaciones() override;
        //Extras
        void mostrarInformacion() const override;

    private:
        int id_peli;
    
};
