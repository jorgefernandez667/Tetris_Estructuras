#ifndef COLAPIEZAS_H
#define COLAPIEZAS_H

#include "Constantes.h"

const int CAPACIDAD_COLA = 16;   // sobra: nunca hay mas de 13 piezas

class ColaPiezas {
private:
    int datos[CAPACIDAD_COLA];
    int frente;
    int fin;
    int n;                       // contador: resuelve "llena o vacia?" cuando frente == fin

    void generarBolsa();         // 7 piezas mezcladas, encoladas al final

public:
    ColaPiezas();                // nace con una bolsa; usa rand(): llamar srand() antes

    bool vacia() const;          // O(1)
    bool llena() const;          // O(1)
    int  tamano() const;         // O(1)

    bool encolar(int tipo);      // O(1)  false si esta llena
    int  desencolar();           // O(1) amortizado; -1 si esta vacia (no ocurre en juego normal)
    int  ver(int i) const;       // O(1)  i = 0 es el frente; -1 si i esta fuera de rango
};

#endif
