// Replay.cpp
#include "Replay.h"

Replay::Replay() {
    primero = ultimo = cursor = nullptr;
    n = 0;
    pos = 0;
}

Replay::~Replay() {
    while (primero != nullptr) {
        NodoReplay* t = primero;
        primero = primero->sig;   // primero avanzar
        delete t;                 // despues liberar
    }
}

void Replay::iniciar(const Estado& e) {
    while (primero != nullptr) {                 // vaciar lo anterior
        NodoReplay* t = primero;
        primero = primero->sig;
        delete t;
    }
    NodoReplay* nuevo = new NodoReplay;
    nuevo->accion = ACC_INICIO;
    nuevo->estado = e;
    nuevo->ant = nullptr;
    nuevo->sig = nullptr;
    primero = ultimo = cursor = nuevo;
    n = 1;
    pos = 0;
}

// Quita todos los nodos a la derecha del cursor, de atras hacia adelante.
void Replay::borrarDespuesDelCursor() {
    while (ultimo != cursor) {
        NodoReplay* t = ultimo;
        ultimo = t->ant;
        ultimo->sig = nullptr;    // 1) reconectar: el nuevo ultimo deja de apuntar a t
        delete t;                 // 2) liberar de ultimo
        n--;
    }
}

void Replay::registrar(int accion, const Estado& e) {
    if (primero == nullptr) return;              // hay que llamar iniciar() antes
    borrarDespuesDelCursor();                    // si habia pasos "rehacer", se pierden

    NodoReplay* nuevo = new NodoReplay;
    nuevo->accion = accion;
    nuevo->estado = e;
    nuevo->sig = nullptr;
    nuevo->ant = ultimo;         // 1) el nuevo mira hacia atras
    ultimo->sig = nuevo;         // 2) el ultimo actual mira hacia adelante
    ultimo = nuevo;              // 3) mover el ultimo
    cursor = nuevo;
    n++;
    pos = n - 1;
}

bool Replay::retroceder() {
    if (cursor == nullptr || cursor->ant == nullptr) return false;
    cursor = cursor->ant;
    pos--;
    return true;
}

bool Replay::avanzar() {
    if (cursor == nullptr || cursor->sig == nullptr) return false;
    cursor = cursor->sig;
    pos++;
    return true;
}

int Replay::retroceder(int pasos) {
    int dados = 0;
    while (dados < pasos && retroceder()) dados++;
    return dados;
}

int Replay::avanzar(int pasos) {
    int dados = 0;
    while (dados < pasos && avanzar()) dados++;
    return dados;
}

void Replay::irAlPrimero() { cursor = primero; pos = 0; }
void Replay::irAlUltimo()  { cursor = ultimo;  pos = (n > 0) ? n - 1 : 0; }

const Estado& Replay::estadoActual() const { return cursor->estado; }
int  Replay::accionActual() const { return cursor->accion; }
int  Replay::posicion() const     { return pos; }
int  Replay::tamano() const       { return n; }
bool Replay::alInicio() const     { return cursor == primero; }
bool Replay::alFinal() const      { return cursor == ultimo; }
