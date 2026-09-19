#include "Tablero.h"

// ---------- construccion y destruccion ----------

Tablero::Tablero() {
    inicio = nullptr;
    for (int i = 0; i < FILAS; i++) insertarFilaVaciaAlInicio();
}

Tablero::~Tablero() {
    while (inicio != nullptr) {
        Fila* t = inicio;
        inicio = inicio->sig;   // primero avanzar
        delete t;               // despues liberar
    }
}

// ---------- auxiliares privadas ----------

void Tablero::insertarFilaVaciaAlInicio() {
    Fila* nueva = new Fila;
    for (int c = 0; c < COLUMNAS; c++) nueva->celdas[c] = 0;
    nueva->sig = inicio;        // 1) la nueva apunta a la antigua cabeza
    inicio = nueva;             // 2) la cabeza pasa a ser la nueva
}

Fila* Tablero::filaEn(int f) const {
    Fila* act = inicio;
    for (int i = 0; i < f && act != nullptr; i++) act = act->sig;
    return act;
}

bool Tablero::filaLlena(const Fila* f) {
    for (int c = 0; c < COLUMNAS; c++)
        if (f->celdas[c] == 0) return false;
    return true;
}

// ---------- consultas ----------

int Tablero::celda(int fila, int col) const {
    if (fila < 0 || fila >= FILAS || col < 0 || col >= COLUMNAS) return -1;
    return filaEn(fila)->celdas[col];
}

bool Tablero::colisiona(const Pieza& p) const {
    for (int i = 0; i < NUM_BLOQUES; i++) {
        Bloque b = bloqueDe(p, i);
        if (b.fila < 0 || b.fila >= FILAS || b.col < 0 || b.col >= COLUMNAS) return true;  // pared o piso
        if (filaEn(b.fila)->celdas[b.col] != 0) return true;                               // celda ocupada
    }
    return false;
}

int Tablero::contarFilas() const {
    int n = 0;
    for (Fila* act = inicio; act != nullptr; act = act->sig) n++;
    return n;
}

// ---------- modificacion ----------

void Tablero::fijar(const Pieza& p) {
    for (int i = 0; i < NUM_BLOQUES; i++) {
        Bloque b = bloqueDe(p, i);
        if (b.fila < 0 || b.fila >= FILAS || b.col < 0 || b.col >= COLUMNAS) continue;
        filaEn(b.fila)->celdas[b.col] = p.tipo + 1;
    }
}

// Recorre la lista con dos punteros (ant, act). Cada fila llena se quita de la lista:
//   1) se avanza 'act'   2) se reconecta   3) se libera (el delete siempre va de ultimo).
// Al final se insertan tantas filas vacias al inicio como filas se quitaron.
int Tablero::limpiarLineas() {
    int eliminadas = 0;
    Fila* ant = nullptr;
    Fila* act = inicio;

    while (act != nullptr) {
        if (filaLlena(act)) {
            Fila* borrar = act;
            act = act->sig;                    // avanzar antes de borrar
            if (ant == nullptr) inicio = act;  // era la cabeza: mover 'inicio'
            else                ant->sig = act;// era intermedia o ultima: reconectar
            delete borrar;                     // ya nadie la alcanza: ahora si
            eliminadas++;
        } else {
            ant = act;
            act = act->sig;
        }
    }

    for (int i = 0; i < eliminadas; i++) insertarFilaVaciaAlInicio();
    return eliminadas;
}

// ---------- soporte para el replay ----------

void Tablero::exportar(int destino[FILAS][COLUMNAS]) const {
    int f = 0;
    for (Fila* act = inicio; act != nullptr; act = act->sig, f++)
        for (int c = 0; c < COLUMNAS; c++) destino[f][c] = act->celdas[c];
}

void Tablero::importar(const int origen[FILAS][COLUMNAS]) {
    int f = 0;
    for (Fila* act = inicio; act != nullptr; act = act->sig, f++)
        for (int c = 0; c < COLUMNAS; c++) act->celdas[c] = origen[f][c];
}
