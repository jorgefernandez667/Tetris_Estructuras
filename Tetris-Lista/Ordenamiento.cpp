#include "Ordenamiento.h"

// ---------- insercion ----------
// Toma cada elemento y lo hunde hacia la izquierda hasta su lugar. Solo desplaza a los que son
// ESTRICTAMENTE menores, por eso los empates no se cruzan (estable).
void ordenarInsercion(Registro a[], int n) {
    for (int i = 1; i < n; i++) {
        Registro clave = a[i];
        int j = i - 1;
        while (j >= 0 && a[j].puntaje < clave.puntaje) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = clave;
    }
}

// ---------- mergesort ----------
// Divide el arreglo a la mitad, ordena cada mitad y las mezcla. El arreglo auxiliar se pide UNA vez.
static void mezclar(Registro a[], Registro aux[], int ini, int med, int fin) {
    int i = ini, j = med, k = ini;                      // mitades [ini,med) y [med,fin)
    while (i < med && j < fin) {
        if (a[j].puntaje > a[i].puntaje) aux[k++] = a[j++];   // la derecha es estrictamente mayor
        else                             aux[k++] = a[i++];   // empate: sale la izquierda (estable)
    }
    while (i < med) aux[k++] = a[i++];
    while (j < fin) aux[k++] = a[j++];
    for (k = ini; k < fin; k++) a[k] = aux[k];
}

static void ordenarMergeRec(Registro a[], Registro aux[], int ini, int fin) {
    if (fin - ini < 2) return;                          // 0 o 1 elemento: ya esta ordenado
    int med = ini + (fin - ini) / 2;
    ordenarMergeRec(a, aux, ini, med);
    ordenarMergeRec(a, aux, med, fin);
    mezclar(a, aux, ini, med, fin);
}

void ordenarMerge(Registro a[], int n) {
    if (n < 2) return;
    Registro* aux = new Registro[n];
    ordenarMergeRec(a, aux, 0, n);
    delete[] aux;
}

// ---------- utilidades ----------

void ordenar(Registro a[], int n, int algoritmo) {
    if (algoritmo == ALG_MERGE) ordenarMerge(a, n);
    else                        ordenarInsercion(a, n);
}

const char* nombreAlgoritmo(int algoritmo) {
    return (algoritmo == ALG_MERGE) ? "Mergesort  O(n log n)" : "Insercion  O(n^2)";
}

bool estaOrdenado(const Registro a[], int n) {
    for (int i = 0; i + 1 < n; i++)
        if (a[i].puntaje < a[i + 1].puntaje) return false;
    return true;
}

// Copia el nombre: sin caracteres de control, sin espacios en los extremos, maximo MAX_NOMBRE
// caracteres. Si queda vacio se llama "Anonimo".
void copiarNombre(Registro& r, const char* nombre) {
    int k = 0;
    if (nombre != 0) {
        int i = 0;
        while (nombre[i] != '\0' && (nombre[i] == ' ' || (unsigned char)nombre[i] < 32)) i++;   // recorta al inicio
        for (; nombre[i] != '\0' && k < MAX_NOMBRE; i++)
            r.nombre[k++] = ((unsigned char)nombre[i] < 32) ? ' ' : nombre[i];
        while (k > 0 && r.nombre[k - 1] == ' ') k--;                                            // recorta al final
    }
    if (k == 0) {
        const char* anonimo = "Anonimo";
        while (anonimo[k] != '\0') { r.nombre[k] = anonimo[k]; k++; }
    }
    r.nombre[k] = '\0';
}

void copiarRegistros(Registro destino[], const Registro origen[], int n) {
    for (int i = 0; i < n; i++) destino[i] = origen[i];
}
