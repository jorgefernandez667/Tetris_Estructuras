#ifndef PUNTAJES_H
#define PUNTAJES_H

#include "Ordenamiento.h"

const int MAX_PUNTAJES = 10;

class Puntajes {
private:
    Registro datos[MAX_PUNTAJES + 1];   // una casilla de sobra: se inserta primero y se recorta despues
    int      n;
    int      algoritmo;                 // ALG_INSERCION o ALG_MERGE

    void insertar(const Registro& r);   // agrega, reordena con el algoritmo elegido y recorta a MAX_PUNTAJES

public:
    Puntajes();

    void setAlgoritmo(int a);           // permite seleccionar cual algoritmo se usa
    int  getAlgoritmo() const;

    bool cargar(const char* ruta);      // false si el archivo no existe (primera vez); ignora lineas malas
    bool guardar(const char* ruta) const;   // false si no se pudo escribir

    bool califica(int puntaje) const;   // true si entra entre los 10 mejores
    bool agregar(const char* nombre, int puntaje);   // false si no califica

    int  tamano() const;
    const Registro& obtener(int i) const;   // i = 0 es el mejor
    void vaciar();
};

#endif
