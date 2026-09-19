#include "PilaHold.h"

PilaHold::PilaHold() {
    tope = -1;
}

bool PilaHold::vacia() const { return tope == -1; }
bool PilaHold::llena() const { return tope == CAPACIDAD_HOLD - 1; }

bool PilaHold::push(int tipo) {
    if (llena()) return false;
    tope++;
    datos[tope] = tipo;
    return true;
}

int PilaHold::pop() {
    if (vacia()) return -1;
    int tipo = datos[tope];
    tope--;
    return tipo;
}

int PilaHold::top() const {
    if (vacia()) return -1;
    return datos[tope];
}
