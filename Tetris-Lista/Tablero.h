#ifndef TABLERO_H
#define TABLERO_H

#include "Constantes.h"
#include "Pieza.h"

struct Fila {
    int   celdas[COLUMNAS];
    Fila* sig;
};

class Tablero {
private:
    Fila* inicio;

    Fila* filaEn(int f) const;              // O(f): recorre desde inicio
    void  insertarFilaVaciaAlInicio();      // O(1) (mas inicializar 10 celdas)
    static bool filaLlena(const Fila* f);   // O(COLUMNAS)

public:
    Tablero();
    ~Tablero();                             // libera TODOS los nodos
    Tablero(const Tablero&) = delete;       // regla de tres: se prohibe la copia
    Tablero& operator=(const Tablero&) = delete;

    int  celda(int fila, int col) const;    // -1 si esta fuera del tablero
    bool colisiona(const Pieza& p) const;   // true si algun bloque se sale o cae en celda ocupada
    void fijar(const Pieza& p);             // escribe los 4 bloques en el tablero
    int  limpiarLineas();                   // elimina filas completas, devuelve cuantas
    bool eliminarFila(int f);               // elimina la fila f (aunque no este completa) e inserta una vacia arriba
    int  contarFilas() const;               // para verificar la invariante (siempre FILAS)

    // Para el replay: copiar el contenido a/desde un arreglo (no crea ni borra nodos).
    void exportar(int destino[FILAS][COLUMNAS]) const;
    void importar(const int origen[FILAS][COLUMNAS]);
};

#endif
