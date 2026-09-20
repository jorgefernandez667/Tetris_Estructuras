#ifndef JUEGO_H
#define JUEGO_H

#include "Constantes.h"
#include "Pieza.h"
#include "Tablero.h"
#include "ColaPiezas.h"
#include "PilaHold.h"
#include "ColaEventos.h"
#include "Estado.h"
#include "Replay.h"

class Juego {
private:
    Tablero     tablero;
    ColaPiezas  cola;
    PilaHold    hold;
    ColaEventos eventos;
    Replay      historial;

    Pieza actual;
    bool  actualEspecial;      // esta pieza limpia una fila al colocarse
    bool  especialPendiente;   // (interno) la proxima pieza en aparecer sera especial
    bool  holdUsado;
    int   puntaje;
    int   lineas;
    int   piezasColocadas;     // el "reloj" de los eventos
    int   nivel;               // 1..NIVEL_MAX: velocidad de caida
    int   bonusRestante;       // piezas que aun dan puntos dobles
    bool  gameOver;

    void aparecer(int tipo);        // crea la pieza; si no cabe -> game over
    void sacarSiguiente();          // la siguiente de la cola pasa a ser la actual
    bool intentarBajar();           // true si bajo; false si no pudo y COLOCO la pieza (no registra)
    void colocar();                 // fija, limpia lineas, puntua, dispara eventos, saca la siguiente
    void programar(int tipo, int momento);
    void dispararEventos();         // aplica los eventos cuyo momento ya llego
    void aplicarEvento(const Evento& e);
    void registrar(int accion);     // agrega un nodo al historial con la foto actual

public:
    Juego();

    // Acciones del jugador. Devuelven true si la accion tuvo efecto.
    bool moverIzquierda();
    bool moverDerecha();
    bool rotar();            // 4 orientaciones precalculadas; si no hay espacio, no rota
    bool bajar();            // baja una fila. Si no puede, FIJA la pieza y devuelve false
    void caidaDura();        // baja hasta el fondo y fija (un solo paso del historial)
    bool usarHold();         // guarda/intercambia la pieza actual; una vez por pieza; no vale con la especial

    // Deshacer / rehacer VARIOS pasos usando la lista doble. Devuelven cuantos pasos dieron.
    int  deshacer(int pasos);
    int  rehacer(int pasos);

    // Fotos del estado (para el historial, las pruebas y para dibujar)
    Estado tomarEstado() const;
    void   cargarEstado(const Estado& e);

    // Consultas
    const Tablero& getTablero() const;
    Pieza  getActual() const;
    bool   esEspecial() const;
    int    holdActual() const;        // tipo guardado, o -1 si esta vacio
    int    siguiente(int i) const;    // i-esima pieza de la cola (0 = la proxima)
    int    getPuntaje() const;
    int    getLineas() const;
    int    getNivel() const;
    int    getPiezasColocadas() const;
    int    getBonusRestante() const;
    int    intervaloCaidaMs() const;  // cada cuantos milisegundos debe caer la pieza (segun el nivel)
    Evento proximoEvento() const;     // el evento del frente de la cola ({-1,-1} si no hay)
    bool   esGameOver() const;
    Replay& getHistorial();           // para reproducir la partida al terminar
};

#endif
