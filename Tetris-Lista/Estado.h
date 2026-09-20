#ifndef ESTADO_H
#define ESTADO_H

#include "Constantes.h"
#include "Pieza.h"
#include "ColaPiezas.h"
#include "ColaEventos.h"

const int MAX_EVENTOS = 8;     // la cola de eventos nunca tiene mas de 3 pendientes

struct Estado {
    int    tablero[FILAS][COLUMNAS];        // 0 vacia, 1..7 = tipo + 1
    Pieza  actual;
    bool   actualEspecial;                  // la pieza actual limpia una fila al colocarse
    int    holdTipo;                        // -1 si el hold esta vacio
    bool   holdUsado;
    int    colaPiezas[CAPACIDAD_COLA];      // piezas futuras, la proxima primero
    int    nColaPiezas;
    int    puntaje;
    int    lineas;
    int    piezasColocadas;                 // reloj de los eventos
    int    nivel;                           // velocidad de caida
    int    bonusRestante;                   // piezas que aun dan puntos dobles
    bool   gameOver;
    Evento eventos[MAX_EVENTOS];            // cola de eventos, el mas proximo primero
    int    nEventos;
};

#endif
