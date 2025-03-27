#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <map>
#include <limits> // Para std::numeric_limits

// Configuración multiplataforma para colores ANSI
void setupConsole() {
    #ifdef _WIN32
    #include <windows.h>
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    #endif
}

// Función auxiliar para limpiar el buffer de entrada
void limpiarBufferEntrada() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Función para obtener entrada numérica válida
int obtenerEntradaNumerica(int min, int max, const std::string& mensaje) {
    int valor;
    while (true) {
        std::cout << mensaje;
        if (!(std::cin >> valor)) {
            limpiarBufferEntrada();
            std::cout << "Entrada no válida. Por favor ingrese un número entre " 
                      << min << " y " << max << ".\n";
        } else if (valor < min || valor > max) {
            std::cout << "Valor fuera de rango. Debe ser entre " 
                      << min << " y " << max << ".\n";
        } else {
            limpiarBufferEntrada();
            return valor;
        }
    }
}

class Carta {
public:
    std::string color;
    int numero;

    Carta(std::string c, int n) : color(c), numero(n) {}

    std::string toString() const {
        static const std::map<std::string, std::string> coloresAnsi = {
            {"Rojo", "\033[31m"},     // Rojo
            {"Azul", "\033[34m"},     // Azul
            {"Amarillo", "\033[33m"}, // Amarillo
            {"Verde", "\033[32m"}     // Verde
        };

        auto it = coloresAnsi.find(color);
        std::string codigoColor = (it != coloresAnsi.end()) ? it->second : "\033[0m";
        return codigoColor + color + " " + std::to_string(numero) + "\033[0m";
    }
};

class Mazo {
private:
    std::vector<Carta> cartas;

public:
    Mazo() {
        crearMazo();
    }

    void crearMazo() {
        const std::string colores[] = {"Rojo", "Azul", "Amarillo", "Verde"};
        for (const auto& color : colores) {
            for (int numero = 1; numero <= 9; ++numero) {
                cartas.push_back(Carta(color, numero));
            }
        }
        barajar();
    }

    void barajar() {
        std::srand(std::time(0));
        std::random_shuffle(cartas.begin(), cartas.end());
    }

    Carta robarCarta() {
        if (estaVacio()) {
            throw std::runtime_error("Error: Se intentó robar carta de mazo vacío");
        }
        Carta carta = cartas.back();
        cartas.pop_back();
        return carta;
    }

    bool estaVacio() const {
        return cartas.empty();
    }

    size_t size() const {
        return cartas.size();
    }
};

class Jugador {
public:
    std::string nombre;
    std::vector<Carta> mano;
    std::vector<Carta> manoInicial;
    int puntuacion;
    int rondasGanadas;

    Jugador(std::string n) : nombre(n), puntuacion(0), rondasGanadas(0) {}

    void mostrarMano() {
        std::cout << "Mano actual de " << nombre << " (" << mano.size() << " cartas):" << std::endl;
        for (size_t i = 0; i < mano.size(); ++i) {
            std::cout << i + 1 << ". " << mano[i].toString() << std::endl;
        }
    }

    void mostrarManoInicial() {
        std::cout << "Mano inicial de " << nombre << " (" << manoInicial.size() << " cartas):" << std::endl;
        for (size_t i = 0; i < manoInicial.size(); ++i) {
            std::cout << i + 1 << ". " << manoInicial[i].toString() << std::endl;
        }
    }

    Carta tirarCarta(int indice) {
        if (indice < 0 || indice >= static_cast<int>(mano.size())) {
            throw std::out_of_range("Índice de carta inválido");
        }
        Carta carta = mano[indice];
        mano.erase(mano.begin() + indice);
        return carta;
    }

    bool tieneColor(const std::string& color) const {
        return std::any_of(mano.begin(), mano.end(), 
            [&color](const Carta& carta) { return carta.color == color; });
    }

    bool tieneCartas() const {
        return !mano.empty();
    }
};

class Juego {
private:
    std::vector<Jugador> jugadores;
    Mazo mazo;
    std::string colorRonda;
    int rondaActual;
    int indiceJugadorInicial;
    int totalRondas;

    void repartirCartas() {
        int cartasPorJugador = 36 / jugadores.size();
        totalRondas = cartasPorJugador;
        
        for (int i = 0; i < cartasPorJugador; ++i) {
            for (auto& jugador : jugadores) {
                try {
                    Carta carta = mazo.robarCarta();
                    jugador.mano.push_back(carta);
                    jugador.manoInicial.push_back(carta);
                } catch (const std::exception& e) {
                    std::cerr << "Error al repartir cartas: " << e.what() << std::endl;
                    throw;
                }
            }
        }
    }

    void mostrarEstadoJugadores() {
        std::cout << "\n=== Estado de los jugadores ===" << std::endl;
        for (auto& jugador : jugadores) {
            std::cout << jugador.nombre << ": " << jugador.mano.size() 
                      << " cartas restantes | Puntos: " << jugador.puntuacion 
                      << " | Rondas ganadas: " << jugador.rondasGanadas << std::endl;
        }
    }

    void definirColorRonda(Jugador& jugador) {
        const std::string coloresValidos[] = {"Rojo", "Azul", "Amarillo", "Verde"};
        bool entradaValida = false;
        
        while (!entradaValida) {
            std::cout << "\n" << jugador.nombre << ", elige el color de la ronda (Rojo/Azul/Amarillo/Verde): ";
            std::cin >> colorRonda;
            
            // Verificar si el color está en la lista de válidos
            for (const auto& color : coloresValidos) {
                if (colorRonda == color) {
                    entradaValida = true;
                    break;
                }
            }
            
            if (!entradaValida) {
                limpiarBufferEntrada();
                std::cout << "Color no válido. Inténtalo de nuevo.\n";
            }
        }
    }

    void jugarRonda() {
        std::vector<Carta> cartasJugadas;
        
        mostrarEstadoJugadores();
        
        for (size_t i = 0; i < jugadores.size(); ++i) {
            Jugador& jugador = jugadores[(indiceJugadorInicial + i) % jugadores.size()];
            
            if (!jugador.tieneCartas()) {
                std::cout << "\n" << jugador.nombre << " no tiene cartas para jugar." << std::endl;
                continue;
            }

            std::cout << "\nTurno de " << jugador.nombre << std::endl;
            jugador.mostrarMano();

            bool debeTirarColor = jugador.tieneColor(colorRonda);
            if (debeTirarColor) {
                std::cout << jugador.nombre << ", debes tirar una carta del color " << colorRonda << "." << std::endl;
            } else {
                std::cout << jugador.nombre << ", no tienes cartas del color " << colorRonda << ". Tira cualquier carta." << std::endl;
            }

            int eleccion = obtenerEntradaNumerica(1, jugador.mano.size(), 
                           "Elige una carta para tirar (1-" + std::to_string(jugador.mano.size()) + "): ");
            
            // Verificar que si debe tirar color, la carta sea del color correcto
            while (debeTirarColor && jugador.mano[eleccion-1].color != colorRonda) {
                std::cout << "Debes elegir una carta del color " << colorRonda << "." << std::endl;
                eleccion = obtenerEntradaNumerica(1, jugador.mano.size(), 
                           "Elige una carta para tirar (1-" + std::to_string(jugador.mano.size()) + "): ");
            }

            try {
                Carta cartaTirada = jugador.tirarCarta(eleccion - 1);
                cartasJugadas.push_back(cartaTirada);
                
                std::cout << "\n" << jugador.nombre << " tiró: " << cartaTirada.toString() << std::endl;
                std::cout << "Mano actualizada:" << std::endl;
                jugador.mostrarMano();
            } catch (const std::exception& e) {
                std::cerr << "Error al tirar carta: " << e.what() << std::endl;
                // Reintentar el turno
                i--;
                continue;
            }
        }

        determinarGanadorRonda(cartasJugadas);
    }

    void determinarGanadorRonda(const std::vector<Carta>& cartasJugadas) {
        int maxPuntos = -1;
        int indiceGanador = -1;

        for (size_t i = 0; i < cartasJugadas.size(); ++i) {
            if (cartasJugadas[i].color == colorRonda && cartasJugadas[i].numero > maxPuntos) {
                maxPuntos = cartasJugadas[i].numero;
                indiceGanador = i;
            }
        }

        if (indiceGanador != -1) {
            int jugadorGanador = (indiceJugadorInicial + indiceGanador) % jugadores.size();
            int puntosRonda = jugadores.size();
            jugadores[jugadorGanador].puntuacion += puntosRonda;
            jugadores[jugadorGanador].rondasGanadas++;
            std::cout << "\n" << jugadores[jugadorGanador].nombre << " gana la ronda y suma " << puntosRonda << " puntos!" << std::endl;
        } else {
            std::cout << "\nNadie gana la ronda." << std::endl;
        }

        indiceJugadorInicial = (indiceJugadorInicial + 1) % jugadores.size();
    }

    void mostrarResultadosFinales() {
        std::cout << "\n\n=== RESULTADOS FINALES ===" << std::endl;
        
        // Ordenar jugadores por puntuación descendente
        std::sort(jugadores.begin(), jugadores.end(), 
            [](const Jugador& a, const Jugador& b) {
                return a.puntuacion > b.puntuacion;
            });
        
        // Mostrar puntuaciones
        for (size_t i = 0; i < jugadores.size(); ++i) {
            std::cout << i + 1 << ". " << jugadores[i].nombre 
                      << " - Puntos: " << jugadores[i].puntuacion 
                      << " - Rondas ganadas: " << jugadores[i].rondasGanadas << std::endl;
        }
        
        // Verificar empates
        if (jugadores.size() > 1 && jugadores[0].puntuacion == jugadores[1].puntuacion) {
            std::cout << "\n¡EMPATE entre: ";
            for (size_t i = 0; i < jugadores.size() && jugadores[i].puntuacion == jugadores[0].puntuacion; ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << jugadores[i].nombre;
            }
            std::cout << "!" << std::endl;
        } else {
            std::cout << "\n¡" << jugadores[0].nombre << " es el GANADOR!" << std::endl;
        }
        
        guardarHistorial();
    }

    void guardarHistorial() {
        try {
            std::ofstream archivo("historial.txt", std::ios::app);
            if (!archivo.is_open()) {
                throw std::runtime_error("No se pudo abrir el archivo de historial");
            }
            
            archivo << "\n=== Partida terminada ===" << std::endl;
            for (const auto& jugador : jugadores) {
                archivo << jugador.nombre << ": " << jugador.puntuacion 
                       << " puntos, " << jugador.rondasGanadas << " rondas ganadas" << std::endl;
            }
            archivo.close();
            std::cout << "Historial guardado correctamente en 'historial.txt'" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error al guardar historial: " << e.what() << std::endl;
        }
    }

public:
    Juego(const std::vector<std::string>& nombresJugadores) : rondaActual(1), indiceJugadorInicial(0), totalRondas(0) {
        for (const std::string& nombre : nombresJugadores) {
            jugadores.push_back(Jugador(nombre));
        }
        try {
            repartirCartas();
        } catch (const std::exception& e) {
            std::cerr << "Error crítico al repartir cartas: " << e.what() << std::endl;
            throw;
        }
    }

    void iniciar() {
        try {
            std::cout << "\n=== Manos iniciales ===" << std::endl;
            for (auto& jugador : jugadores) {
                jugador.mostrarManoInicial();
            }

            while (rondaActual <= totalRondas) {
                std::cout << "\n\n--- Ronda " << rondaActual << " de " << totalRondas << " ---" << std::endl;
                definirColorRonda(jugadores[indiceJugadorInicial]);
                jugarRonda();
                rondaActual++;
            }

            mostrarResultadosFinales();
        } catch (const std::exception& e) {
            std::cerr << "\nERROR CRÍTICO: " << e.what() << std::endl;
            std::cerr << "El juego terminó abruptamente." << std::endl;
        }
    }
};

int main() {
    setupConsole();

    try {
        int numJugadores;
        while (true) {
            std::cout << "¿Cuántos jugadores van a jugar? (2-4): ";
            if (!(std::cin >> numJugadores)) {
                limpiarBufferEntrada();
                std::cout << "Entrada no válida. Debe ser un número entre 2 y 4.\n";
                continue;
            }
            if (numJugadores >= 2 && numJugadores <= 4) {
                break;
            }
            std::cout << "Número de jugadores no válido. Debe ser entre 2 y 4.\n";
        }

        std::vector<std::string> nombresJugadores;
        for (int i = 1; i <= numJugadores; ++i) {
            std::string nombre;
            std::cout << "Nombre del Jugador " << i << ": ";
            std::cin >> nombre;
            nombresJugadores.push_back(nombre);
        }

        Juego juego(nombresJugadores);
        juego.iniciar();
    } catch (const std::exception& e) {
        std::cerr << "\nERROR FATAL: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}