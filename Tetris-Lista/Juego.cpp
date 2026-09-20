#include "Juego.h"

// Puntos segun cuantas lineas se limpian de una vez (0, 1, 2, 3 o 4).
static const int PUNTOS_POR_LINEAS[5] = { 0, 100, 300, 500, 800 };

// Programacion de los eventos (en piezas colocadas)
static const int PRIMER_ACELERAR   = 10;
static const int PRIMER_ESPECIAL   = 7;
static const int PRIMER_DOBLES     = 15;
static const int CADA_ACELERAR     = 10;   // cada cuantas piezas vuelve a acelerar
static const int CADA_ESPECIAL     = 12;
static const int CADA_DOBLES       = 20;
static const int PIEZAS_CON_BONUS  = 5;    // cuantas piezas dan puntos dobles
static const int NIVEL_MAX         = 10;

Juego::Juego() {
    actualEspecial    = false;
    especialPendiente = false;
    holdUsado         = false;
    puntaje           = 0;
    lineas            = 0;
    piezasColocadas   = 0;
    nivel             = 1;
    bonusRestante     = 0;
    gameOver          = false;
    sacarSiguiente();

    // Se insertan a proposito en desorden: la cola los ordena por momento.
    programar(EVENTO_ACELERAR,      PRIMER_ACELERAR);
    programar(EVENTO_PUNTOS_DOBLES, PRIMER_DOBLES);
    programar(EVENTO_PIEZA_ESPECIAL, PRIMER_ESPECIAL);

    historial.iniciar(tomarEstado());         // el primer nodo es el estado inicial
}

// ---------- privadas ----------

void Juego::aparecer(int tipo) {
    actual = nuevaPieza(tipo);
    actualEspecial    = especialPendiente;   // si un evento la marco, esta pieza es especial
    especialPendiente = false;
    if (tablero.colisiona(actual)) gameOver = true;
}

void Juego::sacarSiguiente() {
    aparecer(cola.desencolar());
    holdUsado = false;
}

void Juego::programar(int tipo, int momento) {
    Evento e;
    e.momento = momento;
    e.tipo    = tipo;
    eventos.insertarOrdenado(e);
}

void Juego::aplicarEvento(const Evento& e) {
    switch (e.tipo) {
        case EVENTO_ACELERAR:
            if (nivel < NIVEL_MAX) nivel++;
            programar(EVENTO_ACELERAR, piezasColocadas + CADA_ACELERAR);
            break;
        case EVENTO_PIEZA_ESPECIAL:
            especialPendiente = true;             // la proxima pieza que aparezca
            programar(EVENTO_PIEZA_ESPECIAL, piezasColocadas + CADA_ESPECIAL);
            break;
        case EVENTO_PUNTOS_DOBLES:
            bonusRestante = PIEZAS_CON_BONUS;
            programar(EVENTO_PUNTOS_DOBLES, piezasColocadas + CADA_DOBLES);
            break;
    }
}

// Mientras el evento del frente ya haya llegado a su momento: se extrae y se aplica.
// Termina siempre: cada evento se reprograma en un momento futuro.
void Juego::dispararEventos() {
    while (!eventos.vacia() && eventos.verFrente().momento <= piezasColocadas)
        aplicarEvento(eventos.desencolar());
}

void Juego::colocar() {
    tablero.fijar(actual);

    if (actualEspecial) {                          // pieza especial: elimina la fila de su bloque mas bajo
        int filaBaja = 0;
        for (int i = 0; i < NUM_BLOQUES; i++) {
            Bloque b = bloqueDe(actual, i);
            if (b.fila > filaBaja) filaBaja = b.fila;
        }
        tablero.eliminarFila(filaBaja);
    }

    int k = tablero.limpiarLineas();
    lineas += k;
    int puntos = PUNTOS_POR_LINEAS[k];
    if (bonusRestante > 0) {                       // efecto del evento "puntos dobles"
        puntos *= 2;
        bonusRestante--;
    }
    puntaje += puntos;

    piezasColocadas++;
    dispararEventos();                             // el reloj avanzo: ver si toca algun evento
    sacarSiguiente();
}

bool Juego::intentarBajar() {
    Pieza p = actual;
    p.fila++;
    if (tablero.colisiona(p)) {
        colocar();
        return false;
    }
    actual = p;
    return true;
}

void Juego::registrar(int accion) {
    historial.registrar(accion, tomarEstado());
}

// ---------- acciones ----------

bool Juego::moverIzquierda() {
    if (gameOver) return false;
    Pieza p = actual;
    p.col--;
    if (tablero.colisiona(p)) return false;
    actual = p;
    registrar(ACC_MOVER);
    return true;
}

bool Juego::moverDerecha() {
    if (gameOver) return false;
    Pieza p = actual;
    p.col++;
    if (tablero.colisiona(p)) return false;
    actual = p;
    registrar(ACC_MOVER);
    return true;
}

bool Juego::rotar() {
    if (gameOver) return false;
    Pieza p = actual;
    p.rot = (p.rot + 1) % NUM_ROTACIONES;
    if (tablero.colisiona(p)) return false;   // sin wall kick: si no cabe, no rota
    actual = p;
    registrar(ACC_ROTAR);
    return true;
}

bool Juego::bajar() {
    if (gameOver) return false;
    if (intentarBajar()) {
        registrar(ACC_BAJAR);
        return true;
    }
    registrar(ACC_COLOCAR);                   // no pudo bajar: la pieza quedo fija
    return false;
}

void Juego::caidaDura() {
    if (gameOver) return;
    while (intentarBajar()) { }               // la ultima llamada (false) coloca la pieza
    registrar(ACC_COLOCAR);                   // toda la caida cuenta como UN paso del historial
}

bool Juego::usarHold() {
    if (gameOver || holdUsado || actualEspecial) return false;
    int tipoActual = actual.tipo;
    if (hold.vacia()) {
        hold.push(tipoActual);
        sacarSiguiente();
    } else {
        int guardada = hold.pop();
        hold.push(tipoActual);
        aparecer(guardada);
    }
    holdUsado = true;
    registrar(ACC_HOLD);
    return true;
}

// ---------- deshacer / rehacer ----------

int Juego::deshacer(int pasos) {
    if (gameOver || pasos <= 0) return 0;
    int dados = historial.retroceder(pasos);       // O(pasos) sobre la lista doble
    if (dados > 0) cargarEstado(historial.estadoActual());
    return dados;
}

int Juego::rehacer(int pasos) {
    if (gameOver || pasos <= 0) return 0;
    int dados = historial.avanzar(pasos);
    if (dados > 0) cargarEstado(historial.estadoActual());
    return dados;
}

// ---------- fotos del estado ----------

Estado Juego::tomarEstado() const {
    Estado e;
    tablero.exportar(e.tablero);
    e.actual          = actual;
    e.actualEspecial  = actualEspecial;
    e.holdTipo        = hold.top();
    e.holdUsado       = holdUsado;
    for (int i = 0; i < CAPACIDAD_COLA; i++) e.colaPiezas[i] = -1;
    e.nColaPiezas     = cola.copiarA(e.colaPiezas);
    e.puntaje         = puntaje;
    e.lineas          = lineas;
    e.piezasColocadas = piezasColocadas;
    e.nivel           = nivel;
    e.bonusRestante   = bonusRestante;
    e.gameOver        = gameOver;
    for (int i = 0; i < MAX_EVENTOS; i++) { e.eventos[i].momento = -1; e.eventos[i].tipo = -1; }
    e.nEventos        = eventos.copiarA(e.eventos, MAX_EVENTOS);
    return e;
}

void Juego::cargarEstado(const Estado& e) {
    tablero.importar(e.tablero);
    actual            = e.actual;
    actualEspecial    = e.actualEspecial;
    especialPendiente = false;
    while (!hold.vacia()) hold.pop();
    if (e.holdTipo >= 0) hold.push(e.holdTipo);
    holdUsado         = e.holdUsado;
    cola.cargarDesde(e.colaPiezas, e.nColaPiezas);
    puntaje           = e.puntaje;
    lineas            = e.lineas;
    piezasColocadas   = e.piezasColocadas;
    nivel             = e.nivel;
    bonusRestante     = e.bonusRestante;
    gameOver          = e.gameOver;
    eventos.vaciar();
    for (int i = 0; i < e.nEventos; i++) eventos.insertarOrdenado(e.eventos[i]);
}

// ---------- consultas ----------

const Tablero& Juego::getTablero() const { return tablero; }
Pieza  Juego::getActual() const          { return actual; }
bool   Juego::esEspecial() const         { return actualEspecial; }
int    Juego::holdActual() const         { return hold.top(); }
int    Juego::siguiente(int i) const     { return cola.ver(i); }
int    Juego::getPuntaje() const         { return puntaje; }
int    Juego::getLineas() const          { return lineas; }
int    Juego::getNivel() const           { return nivel; }
int    Juego::getPiezasColocadas() const { return piezasColocadas; }
int    Juego::getBonusRestante() const   { return bonusRestante; }
Evento Juego::proximoEvento() const      { return eventos.verFrente(); }
bool   Juego::esGameOver() const         { return gameOver; }
Replay& Juego::getHistorial()            { return historial; }

int Juego::intervaloCaidaMs() const {
    int ms = 800 - (nivel - 1) * 70;          // nivel 1: 800 ms ... nivel 10: 170 ms
    if (ms < 100) ms = 100;
    return ms;
}
