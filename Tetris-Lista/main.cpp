#include <SFML/Graphics.hpp>
using namespace sf;
// Escriba comandos y presione Enter. Puede escribir varios seguidos, por ejemplo: aaws
//   a = izquierda   d = derecha   w = rotar   s = bajar una fila
//   x = caida dura  c = hold      q = salir
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Juego.h"
using namespace std;

void dibujar(const Juego& j) {
	char cuadro[FILAS][COLUMNAS];
	for (int f = 0; f < FILAS; f++)
		for (int c = 0; c < COLUMNAS; c++) {
			int v = j.getTablero().celda(f, c);
			cuadro[f][c] = (v > 0) ? letraDe(v - 1) : '.';
	}
		if (!j.esGameOver()) {                               // pieza en juego, marcada con '*'
			Pieza p = j.getActual();
			for (int i = 0; i < NUM_BLOQUES; i++) {
				Bloque b = bloqueDe(p, i);
				cuadro[b.fila][b.col] = '*';
			}
		}
		
		cout << "\n";
		for (int f = 0; f < FILAS; f++) {
			cout << "|";
			for (int c = 0; c < COLUMNAS; c++) cout << cuadro[f][c];
			cout << "|";
			if (f == 0) cout << "  Puntaje: " << j.getPuntaje() << "   Lineas: " << j.getLineas();
			if (f == 2) { cout << "  Siguientes: ";
			for (int i = 0; i < 3; i++) cout << letraDe(j.siguiente(i)) << " "; }
			if (f == 4) { cout << "  Hold: ";
			if (j.holdActual() >= 0) cout << letraDe(j.holdActual()); else cout << "-"; }
			cout << "\n";
		}
		cout << "+----------+\n";
}

int main() {
	srand((unsigned)time(NULL));
	Juego juego;
	string linea;
	
	dibujar(juego);
	while (!juego.esGameOver()) {
		cout << "> ";
		if (!getline(cin, linea)) break;
		bool salir = false;
		for (size_t i = 0; i < linea.size(); i++) {
			switch (linea[i]) {
			case 'a': juego.moverIzquierda(); break;
			case 'd': juego.moverDerecha();   break;
			case 'w': juego.rotar();          break;
			case 's': juego.bajar();          break;
			case 'x': juego.caidaDura();      break;
			case 'c': juego.usarHold();       break;
			case 'q': salir = true;           break;
			}
		}
		if (salir) break;
		dibujar(juego);
	}
	if (juego.esGameOver()) { dibujar(juego); cout << "GAME OVER  -  puntaje final: " << juego.getPuntaje() << "\n"; }
	return 0;
}
