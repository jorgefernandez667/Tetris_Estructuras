#ifndef PILAHOLD_H
#define PILAHOLD_H

const int CAPACIDAD_HOLD = 1;

class PilaHold {
private:
    int datos[CAPACIDAD_HOLD];
    int tope;

public:
    PilaHold();

    bool vacia() const;      // O(1)
    bool llena() const;      // O(1)
    bool push(int tipo);     // O(1)  false si esta llena (no se pisa lo guardado)
    int  pop();              // O(1)  retira y devuelve el tope; -1 si esta vacia
    int  top() const;        // O(1)  consulta sin retirar; -1 si esta vacia
};

#endif
