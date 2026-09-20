#include "Interfaz.h"
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <iostream>
using namespace std;

// ---------- constantes de la ventana ----------
static const int   ANCHO = 800;
static const int   ALTO  = 660;
static const int   CELDA = 30;                       // tamano de una celda del tablero, en pixeles
static const float TABLERO_X = 30.f;
static const float TABLERO_Y = 30.f;
static const float PANEL_X   = 370.f;                // columna de la derecha
static const char* ARCHIVO_PUNTAJES = "puntajes.txt";
static const int   MS_ANIMACION = 380;               // duracion del parpadeo de las filas que se limpian
static const int   MS_POR_PASO[4] = { 700, 350, 150, 60 };   // velocidades del replay

static const sf::Color FONDO(12, 12, 22);
static const sf::Color GRIS(150, 150, 175);
static const sf::Color BLANCO(240, 240, 250);
static const sf::Color DORADO(255, 205, 40);

// I, O, T, S, Z, J, L
static const sf::Color COLORES_PIEZA[NUM_PIEZAS] = {
    sf::Color(0, 220, 230), sf::Color(240, 220, 0), sf::Color(170, 60, 230), sf::Color(60, 210, 70),
    sf::Color(230, 50, 50), sf::Color(50, 90, 235), sf::Color(240, 140, 20)
};

// ---------- utilidades ----------

static string aTexto(int v) { ostringstream s; s << v; return s.str(); }   // (std::to_string no existe en el MinGW de ZinjaI)

static const char* nombreAccion(int a) {
    switch (a) {
        case ACC_INICIO:  return "inicio de la partida";
        case ACC_MOVER:   return "mover";
        case ACC_ROTAR:   return "rotar";
        case ACC_BAJAR:   return "bajar";
        case ACC_COLOCAR: return "colocar pieza";
        case ACC_HOLD:    return "hold";
    }
    return "?";
}

static const char* nombreEvento(int t) {
    switch (t) {
        case EVENTO_ACELERAR:       return "Acelerar la caida";
        case EVENTO_PIEZA_ESPECIAL: return "Pieza especial";
        case EVENTO_PUNTOS_DOBLES:  return "Puntos dobles";
    }
    return "-";
}

// true si la pieza cabe en la grilla: dentro de las paredes y sin tocar celdas ocupadas
static bool cabeEnGrilla(const int g[FILAS][COLUMNAS], const Pieza& p) {
    for (int i = 0; i < NUM_BLOQUES; i++) {
        Bloque b = bloqueDe(p, i);
        if (b.fila < 0 || b.fila >= FILAS || b.col < 0 || b.col >= COLUMNAS || g[b.fila][b.col] != 0) return false;
    }
    return true;
}

// ---------- construccion ----------

Interfaz::Interfaz()
    : ventana(sf::VideoMode(ANCHO, ALTO), "Tetris - EIF207 Estructuras de Datos", sf::Style::Titlebar | sf::Style::Close) {
    ventana.setFramerateLimit(60);
    ventana.setKeyRepeatEnabled(false);              // la repeticion de teclas se hace a mano (ver actualizar)
    fuenteOk = cargarFuente();
    if (!fuenteOk) cerr << "AVISO: no se encontro ninguna fuente. Copie un archivo .ttf como 'fuente.ttf' junto al programa.\n";

    juego          = 0;
    pantalla       = PANT_MENU;
    teclaMantenida = -1;
    repitiendo     = false;
    animando       = false;
    calificado     = false;
    resaltado      = -1;
    reproduciendo  = false;
    velocidad      = 2;
    for (int f = 0; f < FILAS; f++) { animFila[f] = false; for (int c = 0; c < COLUMNAS; c++) animTablero[f][c] = 0; }

    puntajes.cargar(ARCHIVO_PUNTAJES);               // si no existe todavia, la tabla queda vacia

    // botones del replay (posicion fija en la columna derecha)
    Boton b0 = { PANEL_X,        470, 52, 38, "" };  botones[B_PRIMERO]  = b0;
    Boton b1 = { PANEL_X + 60,   470, 52, 38, "" };  botones[B_ATRAS]    = b1;
    Boton b2 = { PANEL_X + 120,  470, 96, 38, "" };  botones[B_PLAY]     = b2;
    Boton b3 = { PANEL_X + 224,  470, 52, 38, "" };  botones[B_ADELANTE] = b3;
    Boton b4 = { PANEL_X + 284,  470, 52, 38, "" };  botones[B_ULTIMO]   = b4;
    Boton b5 = { PANEL_X,        522, 84, 34, "" };  botones[B_MENOS]    = b5;
    Boton b6 = { PANEL_X + 92,   522, 84, 34, "" };  botones[B_MAS]      = b6;
    Boton b7 = { PANEL_X + 236,  522, 100, 34, "" }; botones[B_SALIR]    = b7;
}

Interfaz::~Interfaz() {
    delete juego;
}

bool Interfaz::cargarFuente() {
    const char* rutas[] = {
        "fuente.ttf",                                        // la que se entrega junto al programa
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"    // Linux
    };
    for (size_t i = 0; i < sizeof(rutas) / sizeof(rutas[0]); i++)
        if (fuente.loadFromFile(rutas[i])) return true;
    return false;
}

// ---------- primitivas de dibujo ----------

void Interfaz::texto(const string& s, float x, float y, unsigned tam, sf::Color c, int alineacion) {
    if (!fuenteOk) return;
    sf::Text t;
    t.setFont(fuente);
    t.setString(s);
    t.setCharacterSize(tam);
    t.setFillColor(c);
    sf::FloatRect b = t.getLocalBounds();
    float px = x;
    if (alineacion == 1) px = x - b.width / 2.f;
    if (alineacion == 2) px = x - b.width;
    t.setPosition(px, y);
    ventana.draw(t);
}

void Interfaz::rect(float x, float y, float w, float h, sf::Color relleno, sf::Color borde, float grosor) {
    sf::RectangleShape r(sf::Vector2f(w, h));
    r.setPosition(x, y);
    r.setFillColor(relleno);
    if (grosor != 0) { r.setOutlineColor(borde); r.setOutlineThickness(grosor); }
    ventana.draw(r);
}

void Interfaz::celda(float x, float y, float tam, sf::Color color) {
    rect(x + 1, y + 1, tam - 2, tam - 2, color, sf::Color(0, 0, 0, 120), -1.f);
    if (tam >= 20) rect(x + 4, y + 4, tam - 8, 3, sf::Color(255, 255, 255, 70));   // brillo
}

void Interfaz::miniPieza(int tipo, float x, float y, float tam) {
    if (tipo < 0 || tipo >= NUM_PIEZAS) return;
    Pieza p = nuevaPieza(tipo);
    p.fila = 0; p.col = 0;
    for (int i = 0; i < NUM_BLOQUES; i++) {
        Bloque b = bloqueDe(p, i);
        celda(x + b.col * tam, y + b.fila * tam, tam, COLORES_PIEZA[tipo]);
    }
}

void Interfaz::boton(const Boton& b, const string& texto_, bool activo) {
    sf::Vector2i m = sf::Mouse::getPosition(ventana);
    bool encima = (m.x >= b.x && m.x <= b.x + b.w && m.y >= b.y && m.y <= b.y + b.h);
    sf::Color fondo = activo ? sf::Color(60, 90, 160) : sf::Color(38, 40, 62);
    if (encima) fondo = sf::Color(80, 110, 190);
    rect(b.x, b.y, b.w, b.h, fondo, sf::Color(130, 140, 190), 1.f);
    texto(texto_, b.x + b.w / 2.f, b.y + b.h / 2.f - 11, 18, BLANCO, 1);
}

// ---------- tablero ----------
// Dibuja una foto. 'desfase' (0..1) es cuanto ha bajado ya la pieza hacia la siguiente fila:
// asi la caida se ve suave en vez de a saltos.
void Interfaz::dibujarTablero(const Estado& e, bool conPieza, float desfase) {
    float t = relojGlobal.getElapsedTime().asSeconds();
    rect(TABLERO_X, TABLERO_Y, COLUMNAS * CELDA, FILAS * CELDA, sf::Color(8, 8, 16), sf::Color(120, 125, 170), 4.f);
    for (int c = 1; c < COLUMNAS; c++) rect(TABLERO_X + c * CELDA, TABLERO_Y, 1, FILAS * CELDA, sf::Color(255, 255, 255, 16));
    for (int f = 1; f < FILAS; f++)    rect(TABLERO_X, TABLERO_Y + f * CELDA, COLUMNAS * CELDA, 1, sf::Color(255, 255, 255, 16));

    for (int f = 0; f < FILAS; f++)
        for (int c = 0; c < COLUMNAS; c++) {
            int v = e.tablero[f][c];
            if (v > 0) celda(TABLERO_X + c * CELDA, TABLERO_Y + f * CELDA, CELDA, COLORES_PIEZA[v - 1]);
        }

    if (!conPieza || e.gameOver) return;

    // sombra: donde caeria la pieza si se suelta ahora
    Pieza s = e.actual;
    while (true) { Pieza q = s; q.fila++; if (!cabeEnGrilla(e.tablero, q)) break; s = q; }
    if (s.fila != e.actual.fila)
        for (int i = 0; i < NUM_BLOQUES; i++) {
            Bloque b = bloqueDe(s, i);
            sf::Color col = COLORES_PIEZA[s.tipo];
            rect(TABLERO_X + b.col * CELDA + 2, TABLERO_Y + b.fila * CELDA + 2, CELDA - 4, CELDA - 4,
                 sf::Color(col.r, col.g, col.b, 28), sf::Color(col.r, col.g, col.b, 140), 1.f);
        }

    // la pieza en juego (dorada y pulsante si es la especial)
    float pulso = 0.5f + 0.5f * std::sin(t * 9.f);
    for (int i = 0; i < NUM_BLOQUES; i++) {
        Bloque b = bloqueDe(e.actual, i);
        sf::Color col = COLORES_PIEZA[e.actual.tipo];
        if (e.actualEspecial) col = sf::Color(255, (sf::Uint8)(185 + 50 * pulso), (sf::Uint8)(30 + 90 * pulso));
        celda(TABLERO_X + b.col * CELDA, TABLERO_Y + (b.fila + desfase) * CELDA, CELDA, col);
    }
}

// ---------- panel derecho (sirve para la partida y para el replay) ----------
void Interfaz::dibujarPanel(const Estado& e) {
    texto("PUNTAJE", PANEL_X, 26, 14, GRIS);
    texto(aTexto(e.puntaje), PANEL_X, 42, 40, BLANCO);

    texto("LINEAS", PANEL_X, 100, 14, GRIS);          texto(aTexto(e.lineas), PANEL_X, 116, 26, BLANCO);
    texto("NIVEL", PANEL_X + 120, 100, 14, GRIS);     texto(aTexto(e.nivel), PANEL_X + 120, 116, 26, BLANCO);
    texto("PIEZAS", PANEL_X + 240, 100, 14, GRIS);    texto(aTexto(e.piezasColocadas), PANEL_X + 240, 116, 26, BLANCO);

    texto("SIGUIENTES", PANEL_X, 168, 14, GRIS);
    for (int i = 0; i < 3 && i < e.nColaPiezas; i++) miniPieza(e.colaPiezas[i], PANEL_X + i * 92, 192, 20);

    texto("HOLD", PANEL_X, 268, 14, GRIS);
    rect(PANEL_X, 290, 88, 56, sf::Color(20, 20, 34), sf::Color(90, 95, 130), 1.f);
    if (e.holdTipo >= 0) miniPieza(e.holdTipo, PANEL_X + 8, 298, 18);

    texto("PROXIMO EVENTO", PANEL_X + 120, 268, 14, GRIS);
    if (e.nEventos > 0) {
        texto(nombreEvento(e.eventos[0].tipo), PANEL_X + 120, 288, 20, BLANCO);
        int faltan = e.eventos[0].momento - e.piezasColocadas;
        texto("en la pieza " + aTexto(e.eventos[0].momento) + "  (faltan " + aTexto(faltan) + ")", PANEL_X + 120, 314, 15, GRIS);
    }
    if (e.actualEspecial && !e.gameOver) texto("PIEZA ESPECIAL: limpia una fila al colocarse", PANEL_X, 358, 15, DORADO);
    if (e.bonusRestante > 0) texto("PUNTOS DOBLES: " + aTexto(e.bonusRestante) + " piezas", PANEL_X, 380, 17, DORADO);
}

// ---------- pantalla de juego ----------
void Interfaz::dibujarJuego() {
    Estado e = juego->tomarEstado();

    if (animando) {
        // Se muestra el tablero ANTES de quitar las filas, con las completas parpadeando.
        Estado tmp = e;
        for (int f = 0; f < FILAS; f++) for (int c = 0; c < COLUMNAS; c++) tmp.tablero[f][c] = animTablero[f][c];
        dibujarTablero(tmp, false, 0.f);
        float t = relojAnim.getElapsedTime().asSeconds();
        int alfa = (int)(110 + 130 * std::sin(t * 30.f) * std::sin(t * 30.f));
        for (int f = 0; f < FILAS; f++)
            if (animFila[f]) rect(TABLERO_X, TABLERO_Y + f * CELDA, COLUMNAS * CELDA, CELDA, sf::Color(255, 255, 255, (sf::Uint8)alfa));
    } else {
        float desfase = 0.f;
        Pieza abajo = e.actual;
        abajo.fila++;
        if (!e.gameOver && cabeEnGrilla(e.tablero, abajo)) {          // solo se desliza si tiene donde bajar
            desfase = relojCaida.getElapsedTime().asMilliseconds() / (float)juego->intervaloCaidaMs();
            if (desfase > 0.95f) desfase = 0.95f;
        }
        dibujarTablero(e, true, desfase);
    }

    dibujarPanel(e);

    Replay& h = juego->getHistorial();
    texto("HISTORIAL", PANEL_X, 410, 14, GRIS);
    texto("paso " + aTexto(h.posicion()) + " de " + aTexto(h.tamano() - 1), PANEL_X, 428, 18, BLANCO);

    texto("Izquierda / Derecha  o  A / D:  mover", PANEL_X, 470, 14, GRIS);
    texto("Arriba o W:  rotar", PANEL_X, 488, 14, GRIS);
    texto("Abajo o S:  bajar una fila", PANEL_X, 506, 14, GRIS);
    texto("Espacio:  caida", PANEL_X, 524, 14, GRIS);
    texto("C:  hold", PANEL_X, 542, 14, GRIS);
    texto("Z / Y:  deshacer / rehacer  (con Shift, 5 pasos)", PANEL_X, 560, 14, GRIS);
    texto("P o Esc:  pausa", PANEL_X, 578, 14, GRIS);
}

void Interfaz::dibujarPausa() {
    dibujarJuego();
    rect(0, 0, ANCHO, ALTO, sf::Color(0, 0, 0, 175));
    texto("PAUSA", ANCHO / 2.f, 230, 64, BLANCO, 1);
    texto("P o Esc: continuar", ANCHO / 2.f, 330, 24, GRIS, 1);
    texto("M: abandonar y volver al menu", ANCHO / 2.f, 366, 24, GRIS, 1);
}

// ---------- menu ----------
void Interfaz::dibujarMenu() {
    texto("TETRIS", ANCHO / 2.f, 70, 96, BLANCO, 1);
    texto("Estructuras de Datos  -  EIF207  -  Proyecto I", ANCHO / 2.f, 190, 20, GRIS, 1);

    // adorno: las 7 piezas
    for (int i = 0; i < NUM_PIEZAS; i++) miniPieza(i, ANCHO / 2.f - 3.5f * 90 + i * 90 + 26, 250, 16);

    texto("Enter  -  Jugar", ANCHO / 2.f, 340, 30, BLANCO, 1);
    texto("P  -  Mejores puntajes", ANCHO / 2.f, 384, 30, BLANCO, 1);
    texto("Esc  -  Salir", ANCHO / 2.f, 428, 30, BLANCO, 1);
    if (puntajes.tamano() > 0)
        texto("Mejor puntaje: " + aTexto(puntajes.obtener(0).puntaje) + "  (" + puntajes.obtener(0).nombre + ")", ANCHO / 2.f, 520, 18, DORADO, 1);
    if (!fuenteOk) rect(0, 0, ANCHO, 6, sf::Color(230, 50, 50));
}

// ---------- tabla de puntajes ----------
void Interfaz::dibujarTablaPuntajes(float x, float y) {
    texto("MEJORES PUNTAJES", x, y, 22, BLANCO);
    if (puntajes.tamano() == 0) { texto("(todavia no hay puntajes)", x, y + 40, 18, GRIS); return; }
    for (int i = 0; i < puntajes.tamano(); i++) {
        float fy = y + 40 + i * 30;
        if (i == resaltado) rect(x - 10, fy - 2, 330, 28, sf::Color(70, 90, 40, 200));
        sf::Color c = (i == resaltado) ? DORADO : BLANCO;
        texto(aTexto(i + 1) + ".", x, fy, 20, GRIS);
        texto(puntajes.obtener(i).nombre, x + 40, fy, 20, c);
        texto(aTexto(puntajes.obtener(i).puntaje), x + 310, fy, 20, c, 2);
    }
}

void Interfaz::dibujarPuntajes() {
    texto("MEJORES PUNTAJES", ANCHO / 2.f, 40, 44, BLANCO, 1);
    float x = ANCHO / 2.f - 170;
    if (puntajes.tamano() == 0) texto("(todavia no hay puntajes)", ANCHO / 2.f, 200, 22, GRIS, 1);
    for (int i = 0; i < puntajes.tamano(); i++) {
        float fy = 120 + i * 36;
        texto(aTexto(i + 1) + ".", x, fy, 24, GRIS);
        texto(puntajes.obtener(i).nombre, x + 50, fy, 24, BLANCO);
        texto(aTexto(puntajes.obtener(i).puntaje), x + 340, fy, 24, DORADO, 2);
    }
    texto("Algoritmo de ordenamiento:  " + string(nombreAlgoritmo(puntajes.getAlgoritmo())), ANCHO / 2.f, 520, 20, BLANCO, 1);
    texto("A: cambiar algoritmo (se vuelve a ordenar la tabla)      M o Esc: menu", ANCHO / 2.f, 560, 16, GRIS, 1);
}

// ---------- fin de partida ----------
void Interfaz::dibujarFin(bool pidiendoNombre) {
    Estado e = juego->tomarEstado();
    dibujarTablero(e, false, 0.f);
    rect(TABLERO_X, TABLERO_Y, COLUMNAS * CELDA, FILAS * CELDA, sf::Color(0, 0, 0, 120));

    texto("GAME OVER", PANEL_X, 26, 48, sf::Color(235, 70, 70));
    texto("Puntaje final: " + aTexto(e.puntaje), PANEL_X, 92, 26, BLANCO);
    texto("Lineas: " + aTexto(e.lineas) + "     Piezas: " + aTexto(e.piezasColocadas) + "     Nivel: " + aTexto(e.nivel), PANEL_X, 128, 16, GRIS);

    if (pidiendoNombre) {
        texto("Nuevo record! Escribe tu nombre:", PANEL_X, 190, 22, DORADO);
        rect(PANEL_X, 232, 340, 44, sf::Color(20, 20, 34), sf::Color(140, 150, 210), 2.f);
        bool caret = ((int)(relojGlobal.getElapsedTime().asSeconds() * 2)) % 2 == 0;
        texto(nombre + (caret ? "_" : ""), PANEL_X + 10, 238, 26, BLANCO);
        texto("Enter: guardar      Esc: no guardar", PANEL_X, 290, 16, GRIS);
    } else {
        if (calificado) texto("Tu puntaje quedo entre los 10 mejores", PANEL_X, 168, 18, DORADO);
        else            texto("No alcanzo para el top 10", PANEL_X, 168, 18, GRIS);
        dibujarTablaPuntajes(PANEL_X + 10, 200);
        texto("R: ver el replay de la partida", PANEL_X, 560, 18, BLANCO);
        texto("N: nueva partida     P: puntajes     M: menu", PANEL_X, 590, 18, BLANCO);
    }
}

// ---------- replay ----------
void Interfaz::dibujarReplay() {
    Replay& h = juego->getHistorial();
    const Estado& e = h.estadoActual();
    dibujarTablero(e, true, 0.f);
    dibujarPanel(e);

    texto("REPLAY", PANEL_X, 400, 14, GRIS);
    texto("paso " + aTexto(h.posicion()) + " de " + aTexto(h.tamano() - 1) + "   (" + nombreAccion(h.accionActual()) + ")", PANEL_X, 418, 18, BLANCO);
    float fraccion = (h.tamano() > 1) ? (float)h.posicion() / (float)(h.tamano() - 1) : 0.f;
    rect(PANEL_X, 452, 336, 8, sf::Color(30, 32, 50), sf::Color(90, 95, 130), 1.f);
    rect(PANEL_X, 452, 336 * fraccion, 8, sf::Color(90, 150, 240));

    boton(botones[B_PRIMERO],  "|<", false);
    boton(botones[B_ATRAS],    "<", false);
    boton(botones[B_PLAY],     reproduciendo ? "Pausa" : "Play", reproduciendo);
    boton(botones[B_ADELANTE], ">", false);
    boton(botones[B_ULTIMO],   ">|", false);
    boton(botones[B_MENOS],    "Vel -", false);
    boton(botones[B_MAS],      "Vel +", false);
    boton(botones[B_SALIR],    "Salir", false);
    texto("Velocidad " + aTexto(velocidad + 1) + " de 4", PANEL_X, 566, 14, GRIS);
    texto("Teclas:  < >  paso   Espacio  play   Inicio/Fin  extremos   Esc  salir", PANEL_X, 590, 13, GRIS);
    if (e.gameOver) texto("GAME OVER", TABLERO_X + COLUMNAS * CELDA / 2.f, TABLERO_Y + 250, 40, sf::Color(235, 70, 70), 1);
}

// ---------- logica de la interfaz ----------

void Interfaz::nuevaPartida() {
    delete juego;
    juego = new Juego();
    pantalla       = PANT_JUEGO;
    animando       = false;
    teclaMantenida = -1;
    reproduciendo  = false;
    resaltado      = -1;
    relojCaida.restart();
}

void Interfaz::ejecutarOrden(int orden, int pasos) {
    if (juego == 0 || juego->esGameOver() || animando) return;
    Estado antes = juego->tomarEstado();
    switch (orden) {
        case ORD_IZQ:      juego->moverIzquierda(); break;
        case ORD_DER:      juego->moverDerecha();   break;
        case ORD_ROTAR:    juego->rotar();          break;
        case ORD_BAJAR:    juego->bajar();     relojCaida.restart(); break;
        case ORD_CAIDA:    juego->caidaDura(); relojCaida.restart(); break;
        case ORD_HOLD:     juego->usarHold();       break;
        case ORD_DESHACER: juego->deshacer(pasos);  relojCaida.restart(); break;
        case ORD_REHACER:  juego->rehacer(pasos);   relojCaida.restart(); break;
    }
    verificarColocacion(antes);
    if (!animando && juego->esGameOver()) irAFin();
}

// Si la orden coloco una pieza y eso completa filas (o la pieza era especial), se prepara la
// animacion: se reconstruye como habria quedado el tablero justo antes de quitar las filas.
void Interfaz::verificarColocacion(const Estado& antes) {
    if (juego->getPiezasColocadas() <= antes.piezasColocadas) return;      // no se coloco nada

    int grilla[FILAS][COLUMNAS];
    for (int f = 0; f < FILAS; f++) for (int c = 0; c < COLUMNAS; c++) grilla[f][c] = antes.tablero[f][c];

    Pieza p = antes.actual;                                                // la pieza cae hasta donde pueda
    while (true) { Pieza q = p; q.fila++; if (!cabeEnGrilla(grilla, q)) break; p = q; }
    int filaBaja = 0;
    for (int i = 0; i < NUM_BLOQUES; i++) {
        Bloque b = bloqueDe(p, i);
        grilla[b.fila][b.col] = p.tipo + 1;
        if (b.fila > filaBaja) filaBaja = b.fila;
    }

    bool alguna = false;
    for (int f = 0; f < FILAS; f++) {
        animFila[f] = true;
        for (int c = 0; c < COLUMNAS; c++) if (grilla[f][c] == 0) animFila[f] = false;
        if (animFila[f]) alguna = true;
    }
    if (antes.actualEspecial) { animFila[filaBaja] = true; alguna = true; }
    if (!alguna) return;

    for (int f = 0; f < FILAS; f++) for (int c = 0; c < COLUMNAS; c++) animTablero[f][c] = grilla[f][c];
    animando = true;
    relojAnim.restart();
}

void Interfaz::irAFin() {
    teclaMantenida = -1;
    calificado = puntajes.califica(juego->getPuntaje());
    resaltado = -1;
    if (calificado) { nombre.clear(); pantalla = PANT_NOMBRE; relojPantalla.restart(); ventana.setKeyRepeatEnabled(true); }   // se puede mantener Borrar
    else            { pantalla = PANT_FIN; }
}

void Interfaz::guardarPuntaje() {
    puntajes.agregar(nombre.c_str(), juego->getPuntaje());
    puntajes.guardar(ARCHIVO_PUNTAJES);
    // buscar la fila que corresponde a esta partida (con empate queda debajo de los anteriores)
    Registro r; copiarNombre(r, nombre.c_str());
    resaltado = -1;
    for (int i = 0; i < puntajes.tamano(); i++)
        if (puntajes.obtener(i).puntaje == juego->getPuntaje() && string(puntajes.obtener(i).nombre) == string(r.nombre)) resaltado = i;
}

void Interfaz::abrirReplay() {
    juego->getHistorial().irAlPrimero();               // se reproduce desde el primer nodo de la lista
    reproduciendo = false;
    pantalla = PANT_REPLAY;
}

// ---------- eventos ----------

void Interfaz::procesarEvento(const sf::Event& ev) {
    if (ev.type == sf::Event::Closed) { ventana.close(); return; }
    switch (pantalla) {
        case PANT_MENU:     eventoMenu(ev);     break;
        case PANT_JUEGO:    eventoJuego(ev);    break;
        case PANT_PAUSA:    eventoPausa(ev);    break;
        case PANT_NOMBRE:   eventoNombre(ev);   break;
        case PANT_FIN:      eventoFin(ev);      break;
        case PANT_REPLAY:   eventoReplay(ev);   break;
        case PANT_PUNTAJES: eventoPuntajes(ev); break;
    }
}

void Interfaz::eventoMenu(const sf::Event& ev) {
    if (ev.type != sf::Event::KeyPressed) return;
    if (ev.key.code == sf::Keyboard::Return) nuevaPartida();
    if (ev.key.code == sf::Keyboard::P) { puntajes.cargar(ARCHIVO_PUNTAJES); pantalla = PANT_PUNTAJES; }
    if (ev.key.code == sf::Keyboard::Escape) ventana.close();
}

void Interfaz::eventoJuego(const sf::Event& ev) {
    if (ev.type == sf::Event::KeyReleased) {
        if ((int)ev.key.code == teclaMantenida) teclaMantenida = -1;
        return;
    }
    if (ev.type != sf::Event::KeyPressed) return;

    sf::Keyboard::Key k = ev.key.code;
    int pasos = ev.key.shift ? 5 : 1;
    if (k == sf::Keyboard::Left  || k == sf::Keyboard::A) { ejecutarOrden(ORD_IZQ);   teclaMantenida = k; repitiendo = false; relojRepeticion.restart(); }
    else if (k == sf::Keyboard::Right || k == sf::Keyboard::D) { ejecutarOrden(ORD_DER);   teclaMantenida = k; repitiendo = false; relojRepeticion.restart(); }
    else if (k == sf::Keyboard::Down  || k == sf::Keyboard::S) { ejecutarOrden(ORD_BAJAR); teclaMantenida = k; repitiendo = false; relojRepeticion.restart(); }
    else if (k == sf::Keyboard::Up    || k == sf::Keyboard::W) ejecutarOrden(ORD_ROTAR);
    else if (k == sf::Keyboard::Space) ejecutarOrden(ORD_CAIDA);
    else if (k == sf::Keyboard::C)     ejecutarOrden(ORD_HOLD);
    else if (k == sf::Keyboard::Z)     ejecutarOrden(ORD_DESHACER, pasos);
    else if (k == sf::Keyboard::Y)     ejecutarOrden(ORD_REHACER, pasos);
    else if ((k == sf::Keyboard::P || k == sf::Keyboard::Escape) && !animando) { pantalla = PANT_PAUSA; teclaMantenida = -1; }
}

void Interfaz::eventoPausa(const sf::Event& ev) {
    if (ev.type != sf::Event::KeyPressed) return;
    if (ev.key.code == sf::Keyboard::P || ev.key.code == sf::Keyboard::Escape) { pantalla = PANT_JUEGO; relojCaida.restart(); }
    if (ev.key.code == sf::Keyboard::M) pantalla = PANT_MENU;
}

void Interfaz::eventoNombre(const sf::Event& ev) {
    if (ev.type != sf::Event::KeyPressed) return;
    // Un instante de gracia: si el jugador iba soltando piezas con Espacio cuando perdio, esas teclas
    // no deben caer dentro del nombre.
    if (relojPantalla.getElapsedTime().asMilliseconds() < 700) return;
    sf::Keyboard::Key k = ev.key.code;
    if (k == sf::Keyboard::Return) { ventana.setKeyRepeatEnabled(false); guardarPuntaje(); pantalla = PANT_FIN; return; }
    if (k == sf::Keyboard::Escape) { ventana.setKeyRepeatEnabled(false); calificado = false; pantalla = PANT_FIN; return; }
    if (k == sf::Keyboard::BackSpace) { if (!nombre.empty()) nombre.erase(nombre.size() - 1); return; }
    if ((int)nombre.size() >= MAX_NOMBRE) return;
    // solo letras, numeros, espacio y guion: se lee la tecla, no el texto (asi no depende del sistema)
    if (k >= sf::Keyboard::A && k <= sf::Keyboard::Z)               nombre += (char)((ev.key.shift ? 'A' : 'a') + (k - sf::Keyboard::A));
    else if (k >= sf::Keyboard::Num0 && k <= sf::Keyboard::Num9)    nombre += (char)('0' + (k - sf::Keyboard::Num0));
    else if (k >= sf::Keyboard::Numpad0 && k <= sf::Keyboard::Numpad9) nombre += (char)('0' + (k - sf::Keyboard::Numpad0));
    else if (k == sf::Keyboard::Space && !nombre.empty())           nombre += ' ';        // sin espacios al inicio
    else if (k == sf::Keyboard::Dash)                               nombre += (ev.key.shift ? '_' : '-');
}

void Interfaz::eventoFin(const sf::Event& ev) {
    if (ev.type != sf::Event::KeyPressed) return;
    sf::Keyboard::Key k = ev.key.code;
    if (k == sf::Keyboard::R) abrirReplay();
    if (k == sf::Keyboard::N) nuevaPartida();
    if (k == sf::Keyboard::P) { puntajes.cargar(ARCHIVO_PUNTAJES); pantalla = PANT_PUNTAJES; }
    if (k == sf::Keyboard::M || k == sf::Keyboard::Escape) pantalla = PANT_MENU;
}

void Interfaz::pulsarBotonReplay(int id) {
    Replay& h = juego->getHistorial();
    switch (id) {
        case B_PRIMERO:  h.irAlPrimero(); reproduciendo = false; break;
        case B_ATRAS:    h.retroceder();  reproduciendo = false; break;
        case B_ADELANTE: h.avanzar();     reproduciendo = false; break;
        case B_ULTIMO:   h.irAlUltimo();  reproduciendo = false; break;
        case B_PLAY:
            if (!reproduciendo && h.alFinal()) h.irAlPrimero();     // al final, "Play" arranca de nuevo
            reproduciendo = !reproduciendo;
            relojReplay.restart();
            break;
        case B_MENOS: if (velocidad > 0) velocidad--; break;
        case B_MAS:   if (velocidad < 3) velocidad++; break;
        case B_SALIR: reproduciendo = false; pantalla = PANT_FIN; break;
    }
}

void Interfaz::eventoReplay(const sf::Event& ev) {
    if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left) {
        for (int i = 0; i < NUM_BOTONES; i++) {
            const Boton& b = botones[i];
            if (ev.mouseButton.x >= b.x && ev.mouseButton.x <= b.x + b.w && ev.mouseButton.y >= b.y && ev.mouseButton.y <= b.y + b.h)
                pulsarBotonReplay(i);
        }
        return;
    }
    if (ev.type != sf::Event::KeyPressed) return;
    sf::Keyboard::Key k = ev.key.code;
    if (k == sf::Keyboard::Right) pulsarBotonReplay(B_ADELANTE);
    if (k == sf::Keyboard::Left)  pulsarBotonReplay(B_ATRAS);
    if (k == sf::Keyboard::Space) pulsarBotonReplay(B_PLAY);
    if (k == sf::Keyboard::Home)  pulsarBotonReplay(B_PRIMERO);
    if (k == sf::Keyboard::End)   pulsarBotonReplay(B_ULTIMO);
    if (k == sf::Keyboard::Up)    pulsarBotonReplay(B_MAS);
    if (k == sf::Keyboard::Down)  pulsarBotonReplay(B_MENOS);
    if (k == sf::Keyboard::Escape) pulsarBotonReplay(B_SALIR);
}

void Interfaz::eventoPuntajes(const sf::Event& ev) {
    if (ev.type != sf::Event::KeyPressed) return;
    if (ev.key.code == sf::Keyboard::A) {
        puntajes.setAlgoritmo(puntajes.getAlgoritmo() == ALG_MERGE ? ALG_INSERCION : ALG_MERGE);
        puntajes.cargar(ARCHIVO_PUNTAJES);                    // recarga: se vuelve a ordenar con el algoritmo elegido
    }
    if (ev.key.code == sf::Keyboard::M || ev.key.code == sf::Keyboard::Escape || ev.key.code == sf::Keyboard::Return) pantalla = PANT_MENU;
}

// ---------- bucle principal ----------

void Interfaz::actualizar() {
    if (pantalla == PANT_JUEGO && juego != 0) {
        // fin de la animacion de limpieza
        if (animando && relojAnim.getElapsedTime().asMilliseconds() >= MS_ANIMACION) {
            animando = false;
            relojCaida.restart();
            if (juego->esGameOver()) irAFin();
            return;
        }
        if (animando) return;

        // repeticion de teclas mantenidas (izquierda, derecha, abajo)
        if (teclaMantenida != -1) {
            int umbral = repitiendo ? 45 : 170;
            if (relojRepeticion.getElapsedTime().asMilliseconds() >= umbral) {
                if (teclaMantenida == sf::Keyboard::Left  || teclaMantenida == sf::Keyboard::A) ejecutarOrden(ORD_IZQ);
                if (teclaMantenida == sf::Keyboard::Right || teclaMantenida == sf::Keyboard::D) ejecutarOrden(ORD_DER);
                if (teclaMantenida == sf::Keyboard::Down  || teclaMantenida == sf::Keyboard::S) ejecutarOrden(ORD_BAJAR);
                relojRepeticion.restart();
                repitiendo = true;
            }
        }

        // caida automatica segun el nivel
        if (pantalla == PANT_JUEGO && !animando && !juego->esGameOver() &&
            relojCaida.getElapsedTime().asMilliseconds() >= juego->intervaloCaidaMs())
            ejecutarOrden(ORD_BAJAR);
    }

    if (pantalla == PANT_REPLAY && reproduciendo) {
        if (relojReplay.getElapsedTime().asMilliseconds() >= MS_POR_PASO[velocidad]) {
            relojReplay.restart();
            if (!juego->getHistorial().avanzar()) reproduciendo = false;    // llego al final
        }
    }
}

void Interfaz::dibujar() {
    ventana.clear(FONDO);
    switch (pantalla) {
        case PANT_MENU:     dibujarMenu();          break;
        case PANT_JUEGO:    dibujarJuego();         break;
        case PANT_PAUSA:    dibujarPausa();         break;
        case PANT_NOMBRE:   dibujarFin(true);       break;
        case PANT_FIN:      dibujarFin(false);      break;
        case PANT_REPLAY:   dibujarReplay();        break;
        case PANT_PUNTAJES: dibujarPuntajes();      break;
    }
    ventana.display();
}

void Interfaz::ejecutar() {
    while (ventana.isOpen()) {
        sf::Event ev;
        while (ventana.pollEvent(ev)) procesarEvento(ev);
        actualizar();
        dibujar();
    }
}
