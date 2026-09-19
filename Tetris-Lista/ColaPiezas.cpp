#include "ColaPiezas.h"
#include <cstdlib> 

ColaPiezas::ColaPiezas() {
    frente = 0;
    fin    = 0;
    n      = 0;
    generarBolsa();
}

bool ColaPiezas::vacia() const { return n == 0; }
bool ColaPiezas::llena() const { return n == CAPACIDAD_COLA; }
int  ColaPiezas::tamano() const { return n; }

bool ColaPiezas::encolar(int tipo) {
    if (llena()) return false;
    datos[fin] = tipo;
    fin = (fin + 1) % CAPACIDAD_COLA;   // el modulo convierte la linea en anillo
    n++;
    return true;
}

int ColaPiezas::desencolar() {
    if (vacia()) return -1;
    int tipo = datos[frente];
    frente = (frente + 1) % CAPACIDAD_COLA;
    n--;
    if (n < NUM_PIEZAS) generarBolsa(); // quedan menos de 7: agregar otra bolsa (n <= 6, caben 7 mas)
    return tipo;
}

int ColaPiezas::ver(int i) const {
    if (i < 0 || i >= n) return -1;
    return datos[(frente + i) % CAPACIDAD_COLA];
}

// Mezcla de Fisher-Yates: cada una de las 7! ordenes es igual de probable.
void ColaPiezas::generarBolsa() {
    int bolsa[NUM_PIEZAS];
    for (int i = 0; i < NUM_PIEZAS; i++) bolsa[i] = i;

    for (int i = NUM_PIEZAS - 1; i > 0; i--) {
        int j = rand() % (i + 1);    
        int tmp = bolsa[i];
        bolsa[i] = bolsa[j];
        bolsa[j] = tmp;
    }
    for (int i = 0; i < NUM_PIEZAS; i++) encolar(bolsa[i]);
}
