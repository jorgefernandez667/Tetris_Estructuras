#ifndef ORDENAMIENTO_H
#define ORDENAMIENTO_H

const int MAX_NOMBRE = 19;          // caracteres utiles del nombre del jugador

struct Registro {
    char nombre[MAX_NOMBRE + 1];    // texto terminado en '\0'
    int  puntaje;
};

enum Algoritmo { ALG_INSERCION, ALG_MERGE };

void ordenarInsercion(Registro a[], int n);   // O(n^2)
void ordenarMerge(Registro a[], int n);       // O(n log n)
void ordenar(Registro a[], int n, int algoritmo);
const char* nombreAlgoritmo(int algoritmo);

bool estaOrdenado(const Registro a[], int n); // true si esta de mayor a menor (para pruebas)

// Utilidades
void copiarNombre(Registro& r, const char* nombre);                 // recorta, limpia y evita nombres vacios
void copiarRegistros(Registro destino[], const Registro origen[], int n);

#endif
