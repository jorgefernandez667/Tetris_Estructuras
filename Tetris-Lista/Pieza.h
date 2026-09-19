#ifndef PIEZA_H
#define PIEZA_H

#include "Constantes.h"

// Tipos de pieza. Su valor numerico es el indice en la tabla de formas.
enum TipoPieza { PIEZA_I, PIEZA_O, PIEZA_T, PIEZA_S, PIEZA_Z, PIEZA_J, PIEZA_L };

// Una celda del tablero (coordenadas absolutas).
struct Bloque {
    int fila;
    int col;
};

// Pieza en juego: que es, como esta girada y donde esta su caja de 4x4.
struct Pieza {
    int tipo;   // 0..6  (TipoPieza)
    int rot;    // 0..3
    int fila;   // fila de la esquina superior izquierda de la caja
    int col;    // columna de la esquina superior izquierda de la caja
};

// Crea una pieza recien salida: rotacion 0, arriba y centrada.
Pieza nuevaPieza(int tipo);

// Devuelve el bloque i (0..3) de la pieza p, en coordenadas del tablero.
Bloque bloqueDe(const Pieza& p, int i);

// Letra de la pieza ('I','O','T','S','Z','J','L'). Sirve para pruebas y pantalla.
char letraDe(int tipo);

#endif
