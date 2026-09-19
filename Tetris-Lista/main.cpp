#include <SFML/Graphics.hpp>
using namespace sf;
#include <iostream>
#include "Pieza.h"
#include "PilaHold.h"
#include "Tablero.h"
using namespace std;

static int fallos = 0;

void verificar(bool condicion, const char* mensaje) {
	if (!condicion) { cout << "FALLO: " << mensaje << "\n"; fallos++; }
}

void imprimir(const Tablero& t) {
	for (int f = 0; f < FILAS; f++) {
		for (int c = 0; c < COLUMNAS; c++) cout << (t.celda(f, c) ? '#' : '.');
		cout << "\n";
	}
}

bool filaVacia(const Tablero& t, int f) {
	for (int c = 0; c < COLUMNAS; c++) if (t.celda(f, c) != 0) return false;
	return true;
}

// Pone una I horizontal en la fila 'fila' (caja en fila-1), columnas col..col+3
void ponerIHorizontal(Tablero& t, int fila, int col) {
	Pieza p = nuevaPieza(PIEZA_I); p.rot = 0; p.fila = fila - 1; p.col = col;
	verificar(!t.colisiona(p), "I horizontal no deberia colisionar");
	t.fijar(p);
}
// Pone una O que ocupa las filas 'fila' y 'fila+1', columnas col+1..col+2 (la caja empieza en 'col')
void ponerO(Tablero& t, int fila, int col) {
	Pieza p = nuevaPieza(PIEZA_O); p.fila = fila; p.col = col;
	verificar(!t.colisiona(p), "O no deberia colisionar");
	t.fijar(p);
}

int main() {
	// ===== PilaHold =====
	PilaHold h;
	verificar(h.vacia() && !h.llena(), "hold nace vacio");
	verificar(h.pop() == -1 && h.top() == -1, "pop/top sobre vacia dan -1");
	verificar(h.push(PIEZA_T), "push en vacia");
	verificar(h.llena() && h.top() == PIEZA_T, "tras push: llena y top es T");
	verificar(!h.push(PIEZA_L), "push en llena devuelve false");
	verificar(h.top() == PIEZA_T, "el push rechazado no pisa el valor");
	verificar(h.pop() == PIEZA_T && h.vacia(), "pop devuelve T y deja vacia");
	
	// ===== Tablero: estado inicial =====
	Tablero t;
	verificar(t.contarFilas() == FILAS, "20 filas al inicio");
	for (int f = 0; f < FILAS; f++) verificar(filaVacia(t, f), "tablero nuevo vacio");
	verificar(t.celda(-1, 0) == -1 && t.celda(0, COLUMNAS) == -1, "celda fuera de rango");
	
	// ===== Colision: spawn, paredes, piso =====
	for (int tipo = 0; tipo < NUM_PIEZAS; tipo++)
		verificar(!t.colisiona(nuevaPieza(tipo)), "pieza recien creada no colisiona en tablero vacio");
	
	Pieza p = nuevaPieza(PIEZA_I); p.rot = 0;
	p.col = 6;  verificar(!t.colisiona(p), "I horizontal en columnas 6..9 cabe");
	p.col = 7;  verificar(t.colisiona(p),  "I horizontal en columnas 7..10 se sale por la derecha");
	p.col = -1; verificar(t.colisiona(p),  "I horizontal en columna -1 se sale por la izquierda");
	p.col = 3; p.fila = 18; verificar(!t.colisiona(p), "I horizontal apoyada en la fila 19 cabe");
	p.fila = 19;            verificar(t.colisiona(p),  "I horizontal en la fila 20 se sale por el piso");
	
	// ===== Fijar y colision con lo fijado =====
	Pieza o = nuevaPieza(PIEZA_O); o.fila = 18; o.col = 3;   // bloques en filas 18-19, columnas 4-5
	verificar(!t.colisiona(o), "O en el piso cabe");
	t.fijar(o);
	verificar(t.celda(18, 4) == PIEZA_O + 1 && t.celda(19, 5) == PIEZA_O + 1, "celdas fijadas con el color de la O");
	verificar(t.colisiona(o), "misma O ahora colisiona con lo fijado");
	verificar(t.limpiarLineas() == 0, "sin filas llenas no se elimina nada");
	verificar(t.contarFilas() == FILAS, "siguen 20 filas");
	
	// ===== Limpiar 1 linea =====
	Tablero t1;
	ponerIHorizontal(t1, 19, 0);    // fila 19: columnas 0..3
	ponerIHorizontal(t1, 19, 4);    // fila 19: columnas 4..7
	ponerO(t1, 18, 7);              // filas 18-19, columnas 8..9
	cout << "--- antes de limpiar 1 linea (extremo inferior) ---\n"; imprimir(t1);
	int k = t1.limpiarLineas();
	verificar(k == 1, "se elimino 1 linea");
	verificar(t1.contarFilas() == FILAS, "siguen 20 filas tras limpiar 1");
	verificar(t1.celda(19, 8) != 0 && t1.celda(19, 9) != 0, "la fila 18 'cayo' a la 19");
	verificar(t1.celda(19, 0) == 0, "el resto de la nueva fila 19 esta vacio");
	verificar(filaVacia(t1, 0) && filaVacia(t1, 18), "filas vacias arriba");
	
	// ===== Limpiar 4 lineas (tetris) con I verticales =====
	Tablero t4;
	for (int c = 0; c < COLUMNAS; c++) {
		Pieza v = nuevaPieza(PIEZA_I); v.rot = 1; v.fila = 16; v.col = c - 2;   // columna c, filas 16..19
		verificar(!t4.colisiona(v), "I vertical no deberia colisionar");
		t4.fijar(v);
	}
	verificar(t4.limpiarLineas() == 4, "se eliminaron 4 lineas");
	verificar(t4.contarFilas() == FILAS, "siguen 20 filas tras limpiar 4");
	for (int f = 0; f < FILAS; f++) verificar(filaVacia(t4, f), "tablero vacio tras el tetris");
	
	// ===== Dos lineas NO contiguas: filas 19 y 17 llenas, 18 y 16 incompletas =====
	Tablero t2;
	ponerIHorizontal(t2, 19, 0); ponerIHorizontal(t2, 19, 4); ponerO(t2, 18, 7);  // 19 llena, 18 = {8,9}
	ponerIHorizontal(t2, 17, 0); ponerIHorizontal(t2, 17, 4); ponerO(t2, 16, 7);  // 17 llena, 16 = {8,9}
	k = t2.limpiarLineas();
	verificar(k == 2, "se eliminaron 2 lineas no contiguas");
	verificar(t2.contarFilas() == FILAS, "siguen 20 filas tras limpiar 2");
	verificar(t2.celda(19, 8) != 0 && t2.celda(19, 0) == 0, "la antigua fila 18 quedo abajo (fila 19)");
	verificar(t2.celda(18, 8) != 0 && t2.celda(18, 0) == 0, "la antigua fila 16 quedo en la fila 18");
	verificar(filaVacia(t2, 17) && filaVacia(t2, 0), "el resto esta vacio");
	
	// ===== Exportar / importar (para el replay) =====
	int foto[FILAS][COLUMNAS];
	t2.exportar(foto);
	Tablero t3;
	t3.importar(foto);
	bool igual = true;
	for (int f = 0; f < FILAS; f++) for (int c = 0; c < COLUMNAS; c++)
		if (t3.celda(f, c) != t2.celda(f, c)) igual = false;
	verificar(igual, "importar reproduce exactamente lo exportado");
	verificar(t3.contarFilas() == FILAS, "importar no cambia el numero de nodos");
	
	cout << (fallos == 0 ? "TODAS LAS PRUEBAS PASARON" : "HAY FALLOS") << "\n";
	return fallos;
}
