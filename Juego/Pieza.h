#ifndef PIEZA_H
#define PIEZA_H
enum TipoPieza {
	I,
	O,
	T,
	S,
	Z,
	J,
	L
};

class Pieza {
private:
	TipoPieza tipo;
	int orientacion;
	int fila;
	int columna;
	int forma[4][4][4];
	void inicializarFormas();
public:
	Pieza(TipoPieza tipo);
	
	void rotar();
	void mostrarForma() const;
};

#endif
