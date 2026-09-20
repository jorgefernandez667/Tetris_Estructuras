#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <SFML/Graphics.hpp>
#include <string>
#include "Juego.h"
#include "Puntajes.h"

enum Pantalla { PANT_MENU, PANT_JUEGO, PANT_PAUSA, PANT_NOMBRE, PANT_FIN, PANT_REPLAY, PANT_PUNTAJES };

// Ordenes que el jugador puede dar durante la partida
enum Orden { ORD_IZQ, ORD_DER, ORD_ROTAR, ORD_BAJAR, ORD_CAIDA, ORD_HOLD, ORD_DESHACER, ORD_REHACER };

// Botones del replay (se dibujan en pantalla y se pueden pulsar con el raton)
enum IdBoton { B_PRIMERO, B_ATRAS, B_PLAY, B_ADELANTE, B_ULTIMO, B_MENOS, B_MAS, B_SALIR, NUM_BOTONES };

struct Boton {
    float x, y, w, h;
    std::string texto;
};

class Interfaz {
private:
    sf::RenderWindow ventana;
    sf::Font         fuente;
    bool             fuenteOk;
    sf::Clock        relojGlobal;        // para los pulsos y parpadeos

    Juego*    juego;                     // null hasta la primera partida
    Puntajes  puntajes;
    int       pantalla;

    // --- caida y teclas mantenidas ---
    sf::Clock relojCaida;                // tiempo desde la ultima bajada automatica
    int       teclaMantenida;            // -1 si no hay; si no, un sf::Keyboard::Key
    bool      repitiendo;
    sf::Clock relojRepeticion;

    // --- animacion de limpieza de linea ---
    bool      animando;
    sf::Clock relojAnim;
    int       animTablero[FILAS][COLUMNAS];   // el tablero justo antes de quitar las filas
    bool      animFila[FILAS];                // filas que parpadean

    // --- fin de partida ---
    std::string nombre;                  // lo que escribe el jugador
    sf::Clock   relojPantalla;           // desde cuando esta abierta la pantalla de nombre
    bool        calificado;              // entro al top 10
    int         resaltado;               // fila de la tabla que corresponde a esta partida (-1 si ninguna)

    // --- replay ---
    bool      reproduciendo;
    sf::Clock relojReplay;
    int       velocidad;                 // indice en la tabla de milisegundos por paso
    Boton     botones[NUM_BOTONES];

    // ---- dibujo (primitivas) ----
    bool cargarFuente();
    void texto(const std::string& s, float x, float y, unsigned tam, sf::Color c, int alineacion = 0); // 0 izq, 1 centro, 2 der
    void rect(float x, float y, float w, float h, sf::Color relleno, sf::Color borde = sf::Color::Transparent, float grosor = 0);
    void celda(float x, float y, float tam, sf::Color color);
    void miniPieza(int tipo, float x, float y, float tam);
    void boton(const Boton& b, const std::string& texto, bool activo);

    // ---- dibujo (pantallas) ----
    void dibujarTablero(const Estado& e, bool conPieza, float desfase);
    void dibujarPanel(const Estado& e);
    void dibujarJuego();
    void dibujarMenu();
    void dibujarPausa();
    void dibujarFin(bool pidiendoNombre);
    void dibujarReplay();
    void dibujarPuntajes();
    void dibujarTablaPuntajes(float x, float y);

    // ---- logica de la interfaz ----
    void nuevaPartida();
    void ejecutarOrden(int orden, int pasos = 1);
    void verificarColocacion(const Estado& antes);   // decide si hay animacion de limpieza
    void irAFin();
    void guardarPuntaje();
    void abrirReplay();

    void procesarEvento(const sf::Event& ev);
    void eventoMenu(const sf::Event& ev);
    void eventoJuego(const sf::Event& ev);
    void eventoPausa(const sf::Event& ev);
    void eventoNombre(const sf::Event& ev);
    void eventoFin(const sf::Event& ev);
    void eventoReplay(const sf::Event& ev);
    void eventoPuntajes(const sf::Event& ev);
    void pulsarBotonReplay(int id);
    void actualizar();
    void dibujar();

public:
    Interfaz();
    ~Interfaz();
    Interfaz(const Interfaz&) = delete;
    Interfaz& operator=(const Interfaz&) = delete;

    void ejecutar();                     // bucle principal
};

#endif
