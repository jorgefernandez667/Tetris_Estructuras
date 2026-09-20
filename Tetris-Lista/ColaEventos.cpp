#include "ColaEventos.h"

ColaEventos::ColaEventos() {
    frente = nullptr;
    n = 0;
}

ColaEventos::~ColaEventos() {
    vaciar();
}

void ColaEventos::vaciar() {
    while (frente != nullptr) {
        NodoEvento* t = frente;
        frente = frente->sig;   // primero avanzar
        delete t;               // despues liberar
    }
    n = 0;
}

bool ColaEventos::vacia() const  { return n == 0; }
int  ColaEventos::tamano() const { return n; }

// Inserta en la posicion correcta: nunca se inserta al final para reordenar despues.
void ColaEventos::insertarOrdenado(const Evento& e) {
    NodoEvento* nuevo = new NodoEvento;
    nuevo->dato = e;

    // Caso 1: cola vacia o el nuevo es mas proximo que el frente -> va de primero.
    if (frente == nullptr || e.momento < frente->dato.momento) {
        nuevo->sig = frente;
        frente = nuevo;
    } else {
        // Caso 2: caminar hasta el ultimo nodo cuyo momento sea <= al del nuevo
        //         (el <= mantiene el orden de llegada entre empates).
        NodoEvento* act = frente;
        while (act->sig != nullptr && act->sig->dato.momento <= e.momento)
            act = act->sig;
        nuevo->sig = act->sig;  // 1) el nuevo apunta al resto
        act->sig   = nuevo;     // 2) el anterior apunta al nuevo  (el orden es el algoritmo)
    }
    n++;
}

Evento ColaEventos::verFrente() const {
    if (frente == nullptr) { Evento vacio = { -1, -1 }; return vacio; }
    return frente->dato;
}

Evento ColaEventos::desencolar() {
    if (frente == nullptr) { Evento vacio = { -1, -1 }; return vacio; }
    NodoEvento* t = frente;
    Evento e = t->dato;
    frente = frente->sig;       // reconectar
    delete t;                   // liberar de ultimo
    n--;
    return e;
}

int ColaEventos::copiarA(Evento destino[], int maximo) const {
    int k = 0;
    for (NodoEvento* act = frente; act != nullptr && k < maximo; act = act->sig) destino[k++] = act->dato;
    return k;
}
