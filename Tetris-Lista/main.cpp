#include <SFML/Graphics.hpp>
using namespace sf;
// Escriba comandos y presione Enter. Puede escribir varios seguidos, por ejemplo: aaws
//   a = izquierda   d = derecha   w = rotar   s = bajar una fila
//   x = caida dura  c = hold
//   u = deshacer 1 paso    U = deshacer 5 pasos
//   r = rehacer 1 paso     R = rehacer 5 pasos
//   q = salir
// Al terminar la partida se entra al modo REPLAY:  n = siguiente, p = anterior, f = primero, l = ultimo, q = salir
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Juego.h"
using namespace std;

const char* nombreAccion(int a) {
	switch (a) {
	case ACC_INICIO:  return "inicio";
	case ACC_MOVER:   return "mover";
	case ACC_ROTAR:   return "rotar";
	case ACC_BAJAR:   return "bajar";
	case ACC_COLOCAR: return "colocar";
	case ACC_HOLD:    return "hold";
	}
	return "?";
}

const char* nombreEvento(int t) {
	switch (t) {
	case EVENTO_ACELERAR:        return "acelerar";
	case EVENTO_PIEZA_ESPECIAL:  return "pieza especial";
	case EVENTO_PUNTOS_DOBLES:   return "puntos dobles";
	}
	return "-";
}

// Dibuja una foto del juego (sirve igual para la partida en vivo y para el replay)
void dibujar(const Estado& e) {
	char cuadro[FILAS][COLUMNAS];
	for (int f = 0; f < FILAS; f++)
		for (int c = 0; c < COLUMNAS; c++)
			cuadro[f][c] = (e.tablero[f][c] > 0) ? letraDe(e.tablero[f][c] - 1) : '.';
			if (!e.gameOver) {                                   // pieza en juego: '*' normal, '@' especial
				for (int i = 0; i < NUM_BLOQUES; i++) {
					Bloque b = bloqueDe(e.actual, i);
					cuadro[b.fila][b.col] = e.actualEspecial ? '@' : '*';
				}
			}
			
			cout << "\n";
			for (int f = 0; f < FILAS; f++) {
				cout << "|";
				for (int c = 0; c < COLUMNAS; c++) cout << cuadro[f][c];
				cout << "|";
				if (f == 0) cout << "  Puntaje: " << e.puntaje << "   Lineas: " << e.lineas << "   Nivel: " << e.nivel;
				if (f == 1) {
					cout << "  Piezas colocadas: " << e.piezasColocadas;
					if (e.bonusRestante > 0) cout << "   [PUNTOS DOBLES: " << e.bonusRestante << " piezas]";
				}
				if (f == 2) { cout << "  Siguientes: ";
				for (int i = 0; i < 3 && i < e.nColaPiezas; i++) cout << letraDe(e.colaPiezas[i]) << " "; }
				if (f == 4) { cout << "  Hold: ";
				if (e.holdTipo >= 0) cout << letraDe(e.holdTipo); else cout << "-"; }
				if (f == 6 && e.nEventos > 0)
					cout << "  Proximo evento: " << nombreEvento(e.eventos[0].tipo) << " (en la pieza " << e.eventos[0].momento << ")";
				if (f == 7 && e.actualEspecial) cout << "  << PIEZA ESPECIAL: limpia una fila al colocarse >>";
				cout << "\n";
			}
			cout << "+----------+\n";
}

void modoReplay(Juego& juego) {
	Replay& r = juego.getHistorial();
	r.irAlPrimero();
	string linea;
	while (true) {
		dibujar(r.estadoActual());
		cout << "REPLAY  paso " << r.posicion() << " de " << (r.tamano() - 1)
			<< "  (accion: " << nombreAccion(r.accionActual()) << ")\n";
		cout << "[n]siguiente [p]anterior [f]primero [l]ultimo [q]salir > ";
		if (!getline(cin, linea) || linea.empty()) { if (cin.eof()) return; continue; }
		for (size_t i = 0; i < linea.size(); i++) {
			switch (linea[i]) {
			case 'n': r.avanzar();      break;
			case 'p': r.retroceder();   break;
			case 'f': r.irAlPrimero();  break;
			case 'l': r.irAlUltimo();   break;
			case 'q': return;
			}
		}
	}
}

int main() {
	srand((unsigned)time(NULL));
	Juego juego;
	string linea;
	
	dibujar(juego.tomarEstado());
	while (!juego.esGameOver()) {
		cout << "> ";
		if (!getline(cin, linea)) return 0;
		bool salir = false;
		for (size_t i = 0; i < linea.size(); i++) {
			switch (linea[i]) {
			case 'a': juego.moverIzquierda(); break;
			case 'd': juego.moverDerecha();   break;
			case 'w': juego.rotar();          break;
			case 's': juego.bajar();          break;
			case 'x': juego.caidaDura();      break;
			case 'c': juego.usarHold();       break;
			case 'u': juego.deshacer(1);      break;
			case 'U': juego.deshacer(5);      break;
			case 'r': juego.rehacer(1);       break;
			case 'R': juego.rehacer(5);       break;
			case 'q': salir = true;           break;
			}
		}
		if (salir) return 0;
		dibujar(juego.tomarEstado());
	}
	
	cout << "\nGAME OVER  -  puntaje final: " << juego.getPuntaje() << "\n";
	cout << "Entrando al modo replay...\n";
	modoReplay(juego);
	return 0;
}
