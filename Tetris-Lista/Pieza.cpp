#include "Pieza.h"

// FORMAS[tipo][rotacion][bloque] = { fila, columna } dentro de la caja de la pieza.
// Las 4 orientaciones estan escritas a mano (precalculadas): no se calcula nada al rotar.
static const int FORMAS[NUM_PIEZAS][NUM_ROTACIONES][NUM_BLOQUES][2] = {
    { // I
        { {1,0}, {1,1}, {1,2}, {1,3} },
        { {0,2}, {1,2}, {2,2}, {3,2} },
        { {2,0}, {2,1}, {2,2}, {2,3} },
        { {0,1}, {1,1}, {2,1}, {3,1} }
    },
    { // O
        { {0,1}, {0,2}, {1,1}, {1,2} },
        { {0,1}, {0,2}, {1,1}, {1,2} },
        { {0,1}, {0,2}, {1,1}, {1,2} },
        { {0,1}, {0,2}, {1,1}, {1,2} }
    },
    { // T
        { {0,1}, {1,0}, {1,1}, {1,2} },
        { {0,1}, {1,1}, {1,2}, {2,1} },
        { {1,0}, {1,1}, {1,2}, {2,1} },
        { {0,1}, {1,0}, {1,1}, {2,1} }
    },
    { // S
        { {0,1}, {0,2}, {1,0}, {1,1} },
        { {0,1}, {1,1}, {1,2}, {2,2} },
        { {1,1}, {1,2}, {2,0}, {2,1} },
        { {0,0}, {1,0}, {1,1}, {2,1} }
    },
    { // Z
        { {0,0}, {0,1}, {1,1}, {1,2} },
        { {0,2}, {1,1}, {1,2}, {2,1} },
        { {1,0}, {1,1}, {2,1}, {2,2} },
        { {0,1}, {1,0}, {1,1}, {2,0} }
    },
    { // J
        { {0,0}, {1,0}, {1,1}, {1,2} },
        { {0,1}, {0,2}, {1,1}, {2,1} },
        { {1,0}, {1,1}, {1,2}, {2,2} },
        { {0,1}, {1,1}, {2,0}, {2,1} }
    },
    { // L
        { {0,2}, {1,0}, {1,1}, {1,2} },
        { {0,1}, {1,1}, {2,1}, {2,2} },
        { {1,0}, {1,1}, {1,2}, {2,0} },
        { {0,0}, {0,1}, {1,1}, {2,1} }
    }
};

Pieza nuevaPieza(int tipo) {
    Pieza p;
    p.tipo = tipo;
    p.rot  = 0;
    p.fila = 0;
    p.col  = 3;   // la caja de 4 columnas queda en las columnas 3..6
    return p;
}

Bloque bloqueDe(const Pieza& p, int i) {
    Bloque b;
    b.fila = p.fila + FORMAS[p.tipo][p.rot][i][0];
    b.col  = p.col  + FORMAS[p.tipo][p.rot][i][1];
    return b;
}

char letraDe(int tipo) {
    const char letras[NUM_PIEZAS] = { 'I', 'O', 'T', 'S', 'Z', 'J', 'L' };
    return letras[tipo];
}
