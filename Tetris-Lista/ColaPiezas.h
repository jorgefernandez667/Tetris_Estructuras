// Cola FIFO de piezas futuras, con arreglo circular propio (sin std::queue).
//
// INVARIANTES
//   - 0 <= n <= CAPACIDAD_COLA
//   - n == 0  <=>  la cola esta vacia;  n == CAPACIDAD_COLA  <=>  esta llena
//   - 'frente' es el indice del elemento que sale;  'fin' es el indice donde se escribira el proximo
//   - los n elementos estan en frente, frente+1, ... (modulo CAPACIDAD_COLA)
//   - despues de cada desencolar() hay al menos 7 piezas, asi que siempre se pueden ver las 3 proximas
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

    // Para las fotos del replay: sacar/restaurar el contenido en orden (frente primero).
    int  copiarA(int destino[]) const;                 // O(n)  devuelve cuantas piezas copio
    void cargarDesde(const int origen[], int cantidad);// O(n)  reemplaza el contenido de la cola
};

#endif
