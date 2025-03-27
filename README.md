Análisis Detallado del Programa de Juego de Cartas

Este programa implementa un juego de cartas multijugador (2-4 jugadores) donde los participantes compiten en rondas acumulando puntos al jugar cartas del color seleccionado cada ronda. El objetivo es obtener la mayor cantidad de puntos al final de todas las rondas.

Requisitos del Sistema

•	Compilador C++ compatible con C++11 o superior.
•	Sistema operativo: Windows, Linux o macOS.
•	Terminal que soporte códigos de color ANSI (la mayoría de terminales modernos).

Componentes del Programa
El programa se compone de cuatro clases principales

1.	Clase Carta
Representa una carta individual del juego.
Atributos:
•	color: String que indica el color (Rojo, Azul, Amarillo, Verde)
•	numero: Valor numérico de la carta (1-9)
Métodos:
•	toString(): Devuelve representación coloreada de la carta

2.	Clase Mazo
Gestiona el mazo de cartas del juego.

Atributos:

cartas (vector<Carta>):  Almacena todas las cartas del mazo
Métodos;

Constructor Mazo(): Llama a crearMazo() y barajar() para inicializar el mazo listo para jugar.

crearMazo(): Crea un mazo estándar con 4 colores y números del 1 al 9 (36 cartas total), Cada color tiene 9 cartas numeradas.
barajar(): Mezcla aleatoriamente las cartas del mazo usando std::random_shuffle, Usa la hora actual como semilla para el generador aleatorio.
robarCarta(): Toma y devuelve la última carta del mazo (eliminándola del vector).
estaVacio(): Comprueba si el mazo no tiene cartas.
size(): Devuelve el número de cartas restantes en el mazo.

3.	Clase Jugador 
Representa a cada participante del juego.

Atributos:

nombre (string): Nombre del jugador.

mano (vector<Carta>): Cartas actuales en la mano del jugador.

manoInicial (vector<Carta>): Copia de las cartas iniciales (para mostrar al principio).

puntuacion (int): Puntos acumulados durante el juego.

rondasGanadas (int): Número de rondas ganadas.

Métodos:

Constructor Jugador(string n): Inicializa un jugador con nombre, puntuación a 0 y rondas ganadas a 0.

mostrarMano(): Muestra por consola las cartas actuales del jugador con números para selección.

mostrarManoInicial(): Similar a mostrarMano() pero para las cartas iniciales.

tirarCarta(int indice): Elimina y devuelve la carta en la posición especificada.

tieneColor(string color): Comprueba si el jugador tiene alguna carta del color especificado.

tieneCartas(): Devuelve true si el jugador aún tiene cartas en la mano.

4.	Clase Juego
	  Controla la lógica principal del juego.

Atributos:

jugadores (vector<Jugador>): Lista de jugadores participantes.

mazo (Mazo): Mazo de cartas del juego.

colorRonda (string): Color elegido para la ronda actual.

rondaActual (int): Número de ronda actual.

indiceJugadorInicial (int): Índice del jugador que comienza la ronda.

totalRondas (int): Número total de rondas a jugar.

Métodos Privados:
repartirCartas():Distribuye cartas equitativamente a todos los jugadores (36/n_jugadores), Calcula el número total de rondas basado en cartas por jugador, Guarda una copia de la mano inicial para cada jugador.

mostrarEstadoJugadores():Muestra estadísticas actuales: cartas restantes, puntos y rondas ganadas.

definirColorRonda(Jugador& jugador): Pide al jugador inicial que elija un color para la ronda, Valida que el color sea uno de los permitidos.

jugarRonda(): Controla el flujo de una ronda completa: Muestra el estado de los jugadores, Cada jugador juega una carta (siguiendo las reglas del color), Recoge todas las cartas jugadas, Llama a determinarGanadorRonda().

determinarGanadorRonda(vector<Carta>): Compara las cartas jugadas según el color de la ronda, Asigna puntos al ganador (1 punto por jugador), Actualiza el índice del jugador inicial para la siguiente ronda.

mostrarResultadosFinales(): Ordena jugadores por puntuación,Muestra clasificación final, Detecta y muestra empates. Llama a guardarHistorial().

guardarHistorial(): Guarda los resultados en un archivo "historial.txt".


Flujo del Juego
1.	Se crea un mazo de 36 cartas (4 colores × 9 números)
2.	Se reparten cartas equitativamente a los jugadores
3.	Por cada ronda:
o	El jugador inicial elige un color
o	Todos juegan una carta (obligados a seguir el color si pueden)
o	Gana la ronda quien jugó el número más alto del color elegido
o	El ganador suma puntos (1 por cada jugador)
4.	Tras todas las rondas, se muestran los resultados finales

4. Reglas del Juego
1.	Preparación:
o	Se crea un mazo de 36 cartas (4 colores × 9 números)
o	Se reparten cartas equitativamente a los jugadores
2.	Dinámica de ronda:
o	El jugador inicial elige un color para la ronda
o	Todos deben jugar una carta del color elegido si tienen
o	Si no tienen, pueden jugar cualquier carta
o	Gana la ronda quien jugó el número más alto del color elegido
3.	Puntuación:
o	El ganador de cada ronda recibe 1 punto por cada jugador
o	Ejemplo: 4 jugadores → 4 puntos para el ganador
4.	Fin del juego:
o	Termina cuando se han jugado todas las rondas (cartas iniciales / jugadores)
o	Gana quien tenga más puntos al final
![image](https://github.com/user-attachments/assets/787ee6a6-baa9-44a2-8a95-c95c6de3395f)

