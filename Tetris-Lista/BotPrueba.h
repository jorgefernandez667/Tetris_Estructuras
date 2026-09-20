#ifndef BOTPRUEBA_H
#define BOTPRUEBA_H

#include "Juego.h"

// ---------- Jugador automatico (solo para las pruebas) ----------
// Elige donde poner cada pieza mirando el tablero (sin modificarlo) y la coloca con las acciones reales.
inline bool cabeEn(const bool g[FILAS][COLUMNAS], const Pieza& p) {
    for (int i = 0; i < NUM_BLOQUES; i++) {
        Bloque b = bloqueDe(p, i);
        if (b.fila < 0 || b.fila >= FILAS || b.col < 0 || b.col >= COLUMNAS || g[b.fila][b.col]) return false;
    }
    return true;
}

// Puntua una colocacion: premia lineas; castiga altura, huecos y superficie irregular.
inline int evaluar(const bool g0[FILAS][COLUMNAS], const Pieza& p) {
    bool g[FILAS][COLUMNAS];
    for (int f = 0; f < FILAS; f++) for (int c = 0; c < COLUMNAS; c++) g[f][c] = g0[f][c];
    for (int i = 0; i < NUM_BLOQUES; i++) { Bloque b = bloqueDe(p, i); g[b.fila][b.col] = true; }

    int lineasHechas = 0;
    for (int f = 0; f < FILAS; f++) {
        bool llena = true;
        for (int c = 0; c < COLUMNAS; c++) if (!g[f][c]) llena = false;
        if (llena) lineasHechas++;
    }
    int alturas[COLUMNAS], huecos = 0, alturaTotal = 0, irregular = 0;
    for (int c = 0; c < COLUMNAS; c++) {
        alturas[c] = 0;
        bool visto = false;
        for (int f = 0; f < FILAS; f++) {
            if (g[f][c]) { if (!visto) { alturas[c] = FILAS - f; visto = true; } }
            else if (visto) huecos++;
        }
        alturaTotal += alturas[c];
    }
    for (int c = 0; c + 1 < COLUMNAS; c++) {
        int d = alturas[c] - alturas[c + 1];
        irregular += (d < 0) ? -d : d;
    }
    return lineasHechas * 760 - alturaTotal * 510 - huecos * 356 - irregular * 184;
}

// Coloca la pieza actual en la mejor posicion. Devuelve cuantas lineas se limpiaron.
inline int jugarUnaPieza(Juego& j) {
    bool g[FILAS][COLUMNAS];
    for (int f = 0; f < FILAS; f++) for (int c = 0; c < COLUMNAS; c++) g[f][c] = j.getTablero().celda(f, c) > 0;

    Pieza inicial = j.getActual();
    int mejorRot = 0, mejorCol = inicial.col, mejorValor = -1000000;
    for (int r = 0; r < NUM_ROTACIONES; r++)
        for (int c = -3; c < COLUMNAS; c++) {
            Pieza p = inicial; p.rot = r; p.col = c;
            if (!cabeEn(g, p)) continue;
            while (true) { Pieza q = p; q.fila++; if (!cabeEn(g, q)) break; p = q; }
            int v = evaluar(g, p);
            if (v > mejorValor) { mejorValor = v; mejorRot = r; mejorCol = c; }
        }

    int antes = j.getLineas();
    for (int i = 0; i < mejorRot; i++) j.rotar();
    while (j.getActual().col > mejorCol && j.moverIzquierda()) { }
    while (j.getActual().col < mejorCol && j.moverDerecha())   { }
    j.caidaDura();
    return j.getLineas() - antes;
}


#endif
