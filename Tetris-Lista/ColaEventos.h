#ifndef COLAEVENTOS_H
#define COLAEVENTOS_H

// Tipos de evento (documentados en el informe).
enum TipoEvento {
    EVENTO_ACELERAR,        // sube el nivel: la pieza cae mas rapido
    EVENTO_PIEZA_ESPECIAL,  // la proxima pieza, al colocarse, limpia una fila
    EVENTO_PUNTOS_DOBLES    // las proximas piezas dan el doble de puntos
};

struct Evento {
    int momento;   // piezas colocadas en que se dispara
    int tipo;      // TipoEvento
};

struct NodoEvento {
    Evento      dato;
    NodoEvento* sig;
};

class ColaEventos {
private:
    NodoEvento* frente;
    int         n;

public:
    ColaEventos();
    ~ColaEventos();                                  // libera todos los nodos
    ColaEventos(const ColaEventos&) = delete;        // regla de tres: sin copias
    ColaEventos& operator=(const ColaEventos&) = delete;

    void   insertarOrdenado(const Evento& e);  // O(n) peor caso, O(1) mejor caso (va al frente)
    bool   vacia() const;                      // O(1)
    int    tamano() const;                     // O(1)
    Evento verFrente() const;                  // O(1)  {-1,-1} si esta vacia
    Evento desencolar();                       // O(1)  {-1,-1} si esta vacia
    void   vaciar();                           // O(n)

    // Para las fotos del replay
    int copiarA(Evento destino[], int maximo) const;   // O(n) devuelve cuantos copio
};

#endif
