#include "Puntajes.h"
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

Puntajes::Puntajes() {
    n = 0;
    algoritmo = ALG_MERGE;
}

void Puntajes::setAlgoritmo(int a) { algoritmo = (a == ALG_INSERCION) ? ALG_INSERCION : ALG_MERGE; }
int  Puntajes::getAlgoritmo() const { return algoritmo; }
int  Puntajes::tamano() const { return n; }
void Puntajes::vaciar() { n = 0; }

const Registro& Puntajes::obtener(int i) const { return datos[i]; }

// Agrega al final, reordena con el algoritmo elegido y descarta lo que sobre (el peor).
void Puntajes::insertar(const Registro& r) {
    datos[n] = r;
    n++;
    ordenar(datos, n, algoritmo);
    if (n > MAX_PUNTAJES) n = MAX_PUNTAJES;
}

bool Puntajes::califica(int puntaje) const {
    if (puntaje <= 0) return false;                       // una partida sin puntos no entra
    if (n < MAX_PUNTAJES) return true;                    // todavia hay espacio
    return puntaje > datos[n - 1].puntaje;                // debe superar al 10o (un empate no lo desplaza)
}

bool Puntajes::agregar(const char* nombre, int puntaje) {
    if (!califica(puntaje)) return false;
    Registro r;
    copiarNombre(r, nombre);
    r.puntaje = puntaje;
    insertar(r);
    return true;
}

bool Puntajes::cargar(const char* ruta) {
    vaciar();
    ifstream f(ruta);
    if (!f.is_open()) return false;
    string linea;
    while (getline(f, linea)) {
        istringstream iss(linea);
        int puntaje;
        if (!(iss >> puntaje) || puntaje <= 0) continue;  // linea mala: se ignora
        string resto;
        getline(iss, resto);                               // el nombre es el resto de la linea
        Registro r;
        copiarNombre(r, resto.c_str());
        r.puntaje = puntaje;
        insertar(r);                                       // asi el archivo no tiene que venir ordenado
    }
    return true;
}

bool Puntajes::guardar(const char* ruta) const {
    ofstream f(ruta);
    if (!f.is_open()) return false;
    for (int i = 0; i < n; i++) f << datos[i].puntaje << " " << datos[i].nombre << "\n";
    f.close();
    return !f.fail();
}
