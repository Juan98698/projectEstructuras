#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <map>

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
                if (!mazo.estaVacio()) {
                    Carta carta = mazo.robarCarta();
                    jugador.mano.push_back(carta);
                    jugador.manoInicial.push_back(carta);
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
        bool entradaValida = false;
        while (!entradaValida) {
            std::cout << "\n" << jugador.nombre << ", elige el color de la ronda (Rojo, Azul, Amarillo, Verde): ";
            std::cin >> colorRonda;
            
            if (colorRonda == "Rojo" || colorRonda == "Azul" || 
                colorRonda == "Amarillo" || colorRonda == "Verde") {
                entradaValida = true;
            } else {
                std::cout << "Color no válido. Por favor elige entre Rojo, Azul, Amarillo o Verde." << std::endl;
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

            int eleccion;
            bool eleccionValida = false;
            while (!eleccionValida) {
                std::cout << "Elige una carta para tirar (1-" << jugador.mano.size() << "): ";
                std::cin >> eleccion;

                if (eleccion >= 1 && eleccion <= jugador.mano.size()) {
                    if (!debeTirarColor || jugador.mano[eleccion-1].color == colorRonda) {
                        eleccionValida = true;
                    } else {
                        std::cout << "Debes elegir una carta del color " << colorRonda << "." << std::endl;
                    }
                } else {
                    std::cout << "Elección no válida. Por favor elige entre 1 y " << jugador.mano.size() << "." << std::endl;
                }
            }

            Carta cartaTirada = jugador.tirarCarta(eleccion - 1);
            cartasJugadas.push_back(cartaTirada);
            
            std::cout << "\n" << jugador.nombre << " tiró: " << cartaTirada.toString() << std::endl;
            std::cout << "Mano actualizada:" << std::endl;
            jugador.mostrarMano();
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
        std::ofstream archivo("historial.txt", std::ios::app);
        if (archivo.is_open()) {
            archivo << "\n=== Partida terminada ===" << std::endl;
            for (const auto& jugador : jugadores) {
                archivo << jugador.nombre << ": " << jugador.puntuacion 
                         << " puntos, " << jugador.rondasGanadas << " rondas ganadas" << std::endl;
            }
            archivo.close();
        } else {
            std::cerr << "No se pudo abrir el archivo de historial." << std::endl;
        }
    }

public:
    Juego(const std::vector<std::string>& nombresJugadores) : rondaActual(1), indiceJugadorInicial(0), totalRondas(0) {
        for (const std::string& nombre : nombresJugadores) {
            jugadores.push_back(Jugador(nombre));
        }
        repartirCartas();
    }

    void iniciar() {
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
    }
};

int main() {
    setupConsole();

    int numJugadores;
    std::cout << "¿Cuántos jugadores van a jugar? (2-4): ";
    std::cin >> numJugadores;

    if (numJugadores < 2 || numJugadores > 4) {
        std::cerr << "Número de jugadores no válido. Debe ser entre 2 y 4." << std::endl;
        return 1;
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
    return 0;
}