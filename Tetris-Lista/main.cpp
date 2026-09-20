#include <SFML/Graphics.hpp>
using namespace sf;
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Estado.h"
#include "ColaEventos.h"
#include "Replay.h"
using namespace std;

static int fallos = 0;
void verificar(bool condicion, const char* mensaje) {
	if (!condicion) { cout << "FALLO: " << mensaje << "\n"; fallos++; }
}

// Un estado reconocible: solo el puntaje cambia
Estado estadoCon(int puntaje) {
	Estado e;
	for (int f = 0; f < FILAS; f++) for (int c = 0; c < COLUMNAS; c++) e.tablero[f][c] = 0;
	e.actual = nuevaPieza(PIEZA_T);
	e.actualEspecial = false;
	e.holdTipo = -1; e.holdUsado = false;
	e.nColaPiezas = 0;
	e.puntaje = puntaje; e.lineas = 0; e.piezasColocadas = 0; e.nivel = 1; e.bonusRestante = 0;
	e.gameOver = false; e.nEventos = 0;
	return e;
}

// Recorre el replay completo hacia adelante y hacia atras; ambos deben ver los n nodos.
void verificarEnlaces(Replay& r, const char* donde) {
	int guardado = r.posicion();
	r.irAlPrimero();
	int adelante = 1;
	while (r.avanzar()) adelante++;
	int atras = 1;
	while (r.retroceder()) atras++;
	if (adelante != r.tamano() || atras != r.tamano()) {
		cout << "FALLO (enlaces) en: " << donde << "  adelante=" << adelante << " atras=" << atras << " tamano=" << r.tamano() << "\n";
		fallos++;
	}
	r.irAlPrimero();
	r.avanzar(guardado);   // dejar el cursor donde estaba
}

int main() {
	// =============== ColaEventos ===============
	ColaEventos q;
	verificar(q.vacia() && q.tamano() == 0, "cola de eventos nace vacia");
	verificar(q.desencolar().momento == -1 && q.verFrente().momento == -1, "desencolar/ver en vacia da {-1,-1}");
	
	int momentos[] = { 5, 1, 9, 3, 7, 3 };
	int tipos[]    = { EVENTO_ACELERAR, EVENTO_PIEZA_ESPECIAL, EVENTO_PUNTOS_DOBLES,
		EVENTO_ACELERAR, EVENTO_PIEZA_ESPECIAL, EVENTO_PUNTOS_DOBLES };
	for (int i = 0; i < 6; i++) {
		Evento e = { momentos[i], tipos[i] };
		q.insertarOrdenado(e);
		// invariante: el frente es siempre el minimo de lo insertado hasta ahora
		int minimo = momentos[0];
		for (int k = 1; k <= i; k++) if (momentos[k] < minimo) minimo = momentos[k];
		verificar(q.verFrente().momento == minimo, "el frente es siempre el evento mas proximo");
	}
	verificar(q.tamano() == 6, "6 eventos");
	
	Evento copia[MAX_EVENTOS];
	int k = q.copiarA(copia, MAX_EVENTOS);
	verificar(k == 6, "copiarA copia los 6");
	for (int i = 0; i + 1 < k; i++) verificar(copia[i].momento <= copia[i + 1].momento, "orden no decreciente");
	
	int esperado[] = { 1, 3, 3, 5, 7, 9 };
	for (int i = 0; i < 6; i++) {
		Evento e = q.desencolar();
		verificar(e.momento == esperado[i], "salen en orden de momento");
		if (i == 1) verificar(e.tipo == EVENTO_ACELERAR, "empate: sale primero el que llego primero (estable)");
		if (i == 2) verificar(e.tipo == EVENTO_PUNTOS_DOBLES, "empate: sale despues el que llego despues");
	}
	verificar(q.vacia(), "vacia al final");
	
	// insertar al frente, en medio y al final
	Evento a = { 10, EVENTO_ACELERAR }, b = { 20, EVENTO_ACELERAR }, c = { 15, EVENTO_PUNTOS_DOBLES }, d = { 5, EVENTO_PIEZA_ESPECIAL }, z = { 30, EVENTO_ACELERAR };
	q.insertarOrdenado(a); q.insertarOrdenado(b); q.insertarOrdenado(c);   // 10 15 20  (c en medio)
	q.insertarOrdenado(d);                                                 // 5 10 15 20 (al frente)
	q.insertarOrdenado(z);                                                 // 5 10 15 20 30 (al final)
	int esp2[] = { 5, 10, 15, 20, 30 };
	for (int i = 0; i < 5; i++) verificar(q.desencolar().momento == esp2[i], "insercion frente/medio/final");
	
	// reprogramacion como en el juego: sacar y volver a insertar mas adelante
	Evento acel = { 10, EVENTO_ACELERAR }, esp = { 7, EVENTO_PIEZA_ESPECIAL }, dob = { 15, EVENTO_PUNTOS_DOBLES };
	q.insertarOrdenado(acel); q.insertarOrdenado(esp); q.insertarOrdenado(dob);
	Evento sale = q.desencolar();
	verificar(sale.momento == 7 && sale.tipo == EVENTO_PIEZA_ESPECIAL, "primero sale la pieza especial (momento 7)");
	Evento otra = { sale.momento + 12, EVENTO_PIEZA_ESPECIAL };
	q.insertarOrdenado(otra);
	verificar(q.tamano() == 3 && q.verFrente().momento == 10, "tras reprogramar: 10, 15, 19");
	q.vaciar();
	verificar(q.vacia(), "vaciar deja la cola vacia");
	q.insertarOrdenado(a);  // debe quedar usable despues de vaciar
	verificar(q.tamano() == 1, "usable despues de vaciar");
	
	// =============== ColaPiezas: copiarA / cargarDesde ===============
	srand((unsigned)time(NULL));
	ColaPiezas cp;
	cp.desencolar(); cp.desencolar(); cp.desencolar();
	int foto[CAPACIDAD_COLA];
	int nfoto = cp.copiarA(foto);
	verificar(nfoto == cp.tamano(), "copiarA devuelve el tamano");
	int p0 = cp.ver(0), p1 = cp.ver(1);
	cp.desencolar(); cp.desencolar();                       // la cola avanza
	cp.cargarDesde(foto, nfoto);                            // restaurar la foto
	verificar(cp.tamano() == nfoto && cp.ver(0) == p0 && cp.ver(1) == p1, "cargarDesde restaura la cola tal cual");
	int sig = cp.desencolar();
	verificar(sig == p0, "tras restaurar, sale la misma pieza que antes");
	
	// =============== Replay ===============
	Replay r;
	r.iniciar(estadoCon(0));
	verificar(r.tamano() == 1 && r.posicion() == 0 && r.alInicio() && r.alFinal(), "replay nace con 1 nodo (inicio)");
	verificar(r.accionActual() == ACC_INICIO, "primer nodo es ACC_INICIO");
	verificar(!r.retroceder() && !r.avanzar(), "con un solo nodo no hay a donde ir");
	
	// registrar 5 pasos: puntaje 10, 20, 30, 40, 50
	int acciones[] = { ACC_MOVER, ACC_ROTAR, ACC_BAJAR, ACC_COLOCAR, ACC_HOLD };
	for (int i = 0; i < 5; i++) r.registrar(acciones[i], estadoCon((i + 1) * 10));
	verificar(r.tamano() == 6 && r.posicion() == 5 && r.alFinal(), "6 nodos, cursor al final");
	verificar(r.estadoActual().puntaje == 50 && r.accionActual() == ACC_HOLD, "el cursor ve el ultimo estado");
	verificarEnlaces(r, "despues de registrar 5");
	
	// retroceder VARIOS pasos, no solo uno
	int dados = r.retroceder(3);
	verificar(dados == 3 && r.posicion() == 2 && r.estadoActual().puntaje == 20, "retroceder 3 pasos -> puntaje 20");
	verificar(r.accionActual() == ACC_ROTAR, "la accion en la posicion 2 es ROTAR");
	verificar(r.avanzar() && r.estadoActual().puntaje == 30, "rehacer 1 paso -> puntaje 30");
	verificar(r.avanzar(10) == 2 && r.alFinal(), "avanzar(10) solo da los 2 pasos que existen");
	verificar(r.retroceder(100) == 5 && r.alInicio() && r.estadoActual().puntaje == 0, "retroceder(100) llega al inicio con 5 pasos");
	verificar(!r.retroceder(), "no se retrocede antes del inicio");
	verificarEnlaces(r, "despues de ir y volver");
	
	// nueva accion tras deshacer: se descartan los pasos futuros
	r.avanzar(3);                                           // posicion 3 (puntaje 30)
	r.registrar(ACC_MOVER, estadoCon(999));
	verificar(r.tamano() == 5 && r.posicion() == 4 && r.alFinal(), "los pasos futuros se descartaron: 0,10,20,30,999");
	verificar(r.estadoActual().puntaje == 999, "el nodo nuevo es el ultimo");
	verificar(!r.avanzar(), "ya no hay nada que rehacer");
	verificar(r.retroceder() && r.estadoActual().puntaje == 30, "detras del nuevo esta el 30");
	verificarEnlaces(r, "despues de descartar futuros");
	
	// reproduccion completa desde el primer nodo
	r.irAlPrimero();
	int visitados = 1, ultimoPuntaje = r.estadoActual().puntaje;
	bool creciente = true;
	while (r.avanzar()) { visitados++; if (r.estadoActual().puntaje < ultimoPuntaje) creciente = false; ultimoPuntaje = r.estadoActual().puntaje; }
	verificar(visitados == r.tamano() && creciente, "la reproduccion pasa por todos los nodos en orden");
	
	// volver a iniciar libera lo anterior y arranca de cero
	r.iniciar(estadoCon(7));
	verificar(r.tamano() == 1 && r.estadoActual().puntaje == 7, "iniciar de nuevo reinicia el historial");
	
	// muchos nodos y truncados repetidos (estres de memoria; el detector de fugas lo revisa)
	for (int ronda = 0; ronda < 200; ronda++) {
		for (int i = 0; i < 50; i++) r.registrar(ACC_BAJAR, estadoCon(i));
		r.retroceder(rand() % 40);
		r.registrar(ACC_MOVER, estadoCon(-1));
	}
	verificarEnlaces(r, "despues del estres");
	
	cout << (fallos == 0 ? "TODAS LAS PRUEBAS PASARON" : "HAY FALLOS") << "\n";
	return fallos;
}
