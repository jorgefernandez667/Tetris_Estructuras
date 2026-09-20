#ifndef REPLAY_H
#define REPLAY_H

#include "Estado.h"

enum Accion {
    ACC_INICIO,     // estado inicial de la partida (primer nodo)
    ACC_MOVER,
    ACC_ROTAR,
    ACC_BAJAR,
    ACC_COLOCAR,    // la pieza se fijo en el tablero
    ACC_HOLD
};

struct NodoReplay {
    int         accion;
    Estado      estado;
    NodoReplay* ant;
    NodoReplay* sig;
};

class Replay {
private:
    NodoReplay* primero;
    NodoReplay* ultimo;
    NodoReplay* cursor;
    int         n;
    int         pos;

    void borrarDespuesDelCursor();      // descarta los pasos "futuros"

public:
    Replay();
    ~Replay();                                   // libera TODOS los nodos
    Replay(const Replay&) = delete;              // regla de tres: sin copias
    Replay& operator=(const Replay&) = delete;

    void iniciar(const Estado& e);               // borra todo y crea el nodo inicial
    void registrar(int accion, const Estado& e); // agrega al final; si el cursor no esta al final, antes descarta los futuros

    bool retroceder();                           // O(1)  false si ya esta en el primero
    bool avanzar();                              // O(1)  false si ya esta en el ultimo
    int  retroceder(int pasos);                  // O(pasos) devuelve cuantos pasos dio realmente
    int  avanzar(int pasos);                     // O(pasos)
    void irAlPrimero();                          // O(1)  para reproducir desde el inicio
    void irAlUltimo();                           // O(1)

    const Estado& estadoActual() const;          // foto del nodo bajo el cursor
    int  accionActual() const;
    int  posicion() const;                       // indice del cursor (0 = primero)
    int  tamano() const;                         // cantidad de nodos
    bool alInicio() const;
    bool alFinal() const;
};

#endif
