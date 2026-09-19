// Juego.cpp
#include "Juego.h"

// Puntos segun cuantas lineas se limpian de una vez (0, 1, 2, 3 o 4).
static const int PUNTOS_POR_LINEAS[5] = { 0, 100, 300, 500, 800 };

Juego::Juego() {
    puntaje   = 0;
    lineas    = 0;
    gameOver  = false;
    holdUsado = false;
    sacarSiguiente();
}

// ---------- privadas ----------

void Juego::aparecer(int tipo) {
    actual = nuevaPieza(tipo);
    if (tablero.colisiona(actual)) gameOver = true;   // no hay espacio para la pieza nueva
}

void Juego::sacarSiguiente() {
    aparecer(cola.desencolar());
    holdUsado = false;
}

void Juego::colocar() {
    tablero.fijar(actual);
    int k = tablero.limpiarLineas();
    lineas  += k;
    puntaje += PUNTOS_POR_LINEAS[k];
    sacarSiguiente();
}

// ---------- acciones ----------

bool Juego::moverIzquierda() {
    if (gameOver) return false;
    Pieza p = actual;
    p.col--;
    if (tablero.colisiona(p)) return false;
    actual = p;
    return true;
}

bool Juego::moverDerecha() {
    if (gameOver) return false;
    Pieza p = actual;
    p.col++;
    if (tablero.colisiona(p)) return false;
    actual = p;
    return true;
}

bool Juego::rotar() {
    if (gameOver) return false;
    Pieza p = actual;
    p.rot = (p.rot + 1) % NUM_ROTACIONES;
    if (tablero.colisiona(p)) return false;   // sin wall kick: si no cabe, no rota
    actual = p;
    return true;
}

bool Juego::bajar() {
    if (gameOver) return false;
    Pieza p = actual;
    p.fila++;
    if (tablero.colisiona(p)) {
        colocar();                            // no puede bajar mas: queda fija
        return false;
    }
    actual = p;
    return true;
}

void Juego::caidaDura() {
    if (gameOver) return;
    while (bajar()) { }                       // la ultima llamada (false) fija la pieza
}

bool Juego::usarHold() {
    if (gameOver || holdUsado) return false;
    int tipoActual = actual.tipo;
    if (hold.vacia()) {
        hold.push(tipoActual);
        sacarSiguiente();                     // la pieza nueva viene de la cola
    } else {
        int guardada = hold.pop();            // intercambio: sale la vieja, entra la actual
        hold.push(tipoActual);
        aparecer(guardada);
    }
    holdUsado = true;                         // despues de aparecer: bloquea un segundo hold seguido
    return true;
}

// ---------- consultas ----------

const Tablero& Juego::getTablero() const { return tablero; }
Pieza Juego::getActual() const           { return actual; }
int   Juego::holdActual() const          { return hold.top(); }
int   Juego::siguiente(int i) const      { return cola.ver(i); }
int   Juego::getPuntaje() const          { return puntaje; }
int   Juego::getLineas() const           { return lineas; }
bool  Juego::esGameOver() const          { return gameOver; }
