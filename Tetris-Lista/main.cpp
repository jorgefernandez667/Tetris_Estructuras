#include <SFML/Graphics.hpp>
using namespace sf;
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Pieza.h"
#include "ColaPiezas.h"
using namespace std;

// Dibuja una pieza en su caja de 4x4
void dibujar(int tipo, int rot) {
	Pieza p = nuevaPieza(tipo);
	p.rot = rot; p.fila = 0; p.col = 0;
	char caja[4][4];
	for (int f = 0; f < 4; f++) for (int c = 0; c < 4; c++) caja[f][c] = '.';
	for (int i = 0; i < NUM_BLOQUES; i++) {
		Bloque b = bloqueDe(p, i);
		caja[b.fila][b.col] = letraDe(tipo);
	}
	for (int f = 0; f < 4; f++) {
		for (int c = 0; c < 4; c++) cout << caja[f][c];
		cout << "\n";
	}
}

int main() {
	srand((unsigned)time(NULL));
	int fallos = 0;
	
	// --- Prueba 1: las 4 orientaciones de la T y de la I ---
	cout << "=== Pieza T, 4 rotaciones ===\n";
	for (int r = 0; r < 4; r++) { cout << "rot " << r << "\n"; dibujar(PIEZA_T, r); }
	cout << "=== Pieza I, 4 rotaciones ===\n";
	for (int r = 0; r < 4; r++) { cout << "rot " << r << "\n"; dibujar(PIEZA_I, r); }
	
	// --- Prueba 2: toda pieza tiene 4 bloques distintos dentro de su caja ---
	for (int t = 0; t < NUM_PIEZAS; t++)
		for (int r = 0; r < NUM_ROTACIONES; r++) {
			Pieza p = nuevaPieza(t); p.rot = r; p.fila = 0; p.col = 0;
			for (int i = 0; i < NUM_BLOQUES; i++) {
				Bloque a = bloqueDe(p, i);
				if (a.fila < 0 || a.fila > 3 || a.col < 0 || a.col > 3) { cout << "FALLO: fuera de la caja\n"; fallos++; }
				for (int j = i + 1; j < NUM_BLOQUES; j++) {
					Bloque b = bloqueDe(p, j);
					if (a.fila == b.fila && a.col == b.col) { cout << "FALLO: bloques repetidos\n"; fallos++; }
				}
			}
	}
		
		// --- Prueba 3: la cola entrega bolsas de 7 (cada grupo de 7 tiene las 7 piezas) ---
		ColaPiezas cola;
		for (int bolsa = 0; bolsa < 100; bolsa++) {
			int cuenta[NUM_PIEZAS] = {0};
			for (int k = 0; k < NUM_PIEZAS; k++) {
				int v = cola.ver(0);
				int s = cola.desencolar();
				if (v != s) { cout << "FALLO: ver(0) != desencolar()\n"; fallos++; }
				cuenta[s]++;
			}
			for (int t = 0; t < NUM_PIEZAS; t++)
				if (cuenta[t] != 1) { cout << "FALLO: la bolsa " << bolsa << " no tiene cada pieza una vez\n"; fallos++; }
				if (cola.tamano() < 7 || cola.tamano() > CAPACIDAD_COLA) { cout << "FALLO: tamano fuera de rango\n"; fallos++; }
		}
		
		// --- Prueba 4: vista previa de las 3 siguientes ---
		cout << "Proximas 3: ";
		for (int i = 0; i < 3; i++) cout << letraDe(cola.ver(i)) << " ";
		cout << "\n";
		
		// --- Prueba 5: bordes ---
		if (cola.ver(-1) != -1 || cola.ver(cola.tamano()) != -1) { cout << "FALLO: ver fuera de rango\n"; fallos++; }
		ColaPiezas c2;
		while (c2.tamano() < CAPACIDAD_COLA) c2.encolar(0);
		if (c2.encolar(0)) { cout << "FALLO: encolar en cola llena\n"; fallos++; }
		
		cout << (fallos == 0 ? "TODAS LAS PRUEBAS PASARON" : "HAY FALLOS") << "\n";
		return fallos;
}
