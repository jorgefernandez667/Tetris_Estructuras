// Juego.h
// Logica del juego (sin SFML): junta Tablero, ColaPiezas y PilaHold.
// Aqui viven las reglas: mover, rotar, bajar, hold, fijar pieza, puntaje y game over.
//
// INVARIANTES
//   - mientras !gameOver: la pieza 'actual' NO colisiona con el tablero
//   - el tablero siempre tiene FILAS nodos (lo garantiza Tablero)
//   - holdUsado == true  <=>  ya se uso el hold con la pieza actual (solo se permite una vez por pieza)
//   - puntaje y lineas nunca disminuyen
//   - gameOver == true  =>  toda accion devuelve false y no cambia nada
#ifndef JUEGO_H
#define JUEGO_H

#include "Constantes.h"
#include "Pieza.h"
#include "Tablero.h"
#include "ColaPiezas.h"
#include "PilaHold.h"

class Juego {
private:
    Tablero    tablero;
    ColaPiezas cola;
    PilaHold   hold;
    Pieza      actual;
    bool       holdUsado;
    int        puntaje;
    int        lineas;
    bool       gameOver;

    void sacarSiguiente();   // la siguiente de la cola pasa a ser la pieza actual
    void aparecer(int tipo); // crea la pieza en su posicion inicial; si no cabe -> game over
    void colocar();          // fija la pieza, limpia lineas, suma puntos y saca la siguiente

public:
    Juego();

    // Acciones del jugador. Devuelven true si la accion tuvo efecto.
    bool moverIzquierda();
    bool moverDerecha();
    bool rotar();            // 4 orientaciones precalculadas; si no hay espacio, no rota
    bool bajar();            // baja una fila. Si no puede, FIJA la pieza y devuelve false
    void caidaDura();        // baja hasta el fondo y fija
    bool usarHold();         // guarda la pieza actual (o la intercambia); una vez por pieza

    // Consultas (para dibujar y para las pruebas)
    const Tablero& getTablero() const;
    Pieza getActual() const;
    int   holdActual() const;        // tipo guardado, o -1 si esta vacio
    int   siguiente(int i) const;    // i-esima pieza de la cola (0 = la proxima)
    int   getPuntaje() const;
    int   getLineas() const;
    bool  esGameOver() const;
};

#endif
