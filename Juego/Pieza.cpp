#include "Pieza.h"
#include <iostream>


void Pieza::rotar() {
	orientacion++;
	
	if (orientacion == 4) {
		orientacion = 0;
	}
}
void Pieza::mostrarForma() const {
	for (int f = 0; f < 4; f++) {
		for (int c = 0; c < 4; c++) {
			std::cout << forma[orientacion][f][c] << " ";
		}
		std::cout << std::endl;
	}
}
Pieza::Pieza(TipoPieza tipo) {
	this->tipo = tipo;
	orientacion = 0;
	fila = 0;
	columna = 0;
	inicializarFormas();
}
	void Pieza::inicializarFormas() {
		
		for (int o = 0; o < 4; o++) {
			for (int f = 0; f < 4; f++) {
				for (int c = 0; c < 4; c++) {
					forma[o][f][c] = 0;
				}
			}
		}
	

	
if (tipo == I) {
	
	forma[0][1][0] = 1;
	forma[0][1][1] = 1;
	forma[0][1][2] = 1;
	forma[0][1][3] = 1;
	
	forma[1][0][1] = 1;
	forma[1][1][1] = 1;
	forma[1][2][1] = 1;
	forma[1][3][1] = 1;
	
	forma[2][2][0] = 1;
	forma[2][2][1] = 1;
	forma[2][2][2] = 1;
	forma[2][2][3] = 1;
	
	forma[3][0][2] = 1;
	forma[3][1][2] = 1;
	forma[3][2][2] = 1;
	forma[3][3][2] = 1;
	
} else if (tipo == O) {
	
	for (int o = 0; o < 4; o++) {
		forma[o][1][1] = 1;
		forma[o][1][2] = 1;
		forma[o][2][1] = 1;
		forma[o][2][2] = 1;
	}
}else if (tipo == T) {
	
	forma[0][0][1] = 1;
	forma[0][1][0] = 1;
	forma[0][1][1] = 1;
	forma[0][1][2] = 1;

	forma[1][0][1] = 1;
	forma[1][1][1] = 1;
	forma[1][1][2] = 1;
	forma[1][2][1] = 1;

	forma[2][1][0] = 1;
	forma[2][1][1] = 1;
	forma[2][1][2] = 1;
	forma[2][2][1] = 1;

	forma[3][0][1] = 1;
	forma[3][1][0] = 1;
	forma[3][1][1] = 1;
	forma[3][2][1] = 1;
	
}else if (tipo == S) {
	
	forma[0][0][1] = 1;
	forma[0][0][2] = 1;
	forma[0][1][0] = 1;
	forma[0][1][1] = 1;
	
	forma[1][0][0] = 1;
	forma[1][1][0] = 1;
	forma[1][1][1] = 1;
	forma[1][2][1] = 1;

	forma[2][1][1] = 1;
	forma[2][1][2] = 1;
	forma[2][2][0] = 1;
	forma[2][2][1] = 1;

	forma[3][0][0] = 1;
	forma[3][1][0] = 1;
	forma[3][1][1] = 1;
	forma[3][2][1] = 1;
	
}else if (tipo == Z) {
	
	forma[0][0][0] = 1;
	forma[0][0][1] = 1;
	forma[0][1][1] = 1;
	forma[0][1][2] = 1;
	
	forma[1][0][1] = 1;
	forma[1][1][0] = 1;
	forma[1][1][1] = 1;
	forma[1][2][0] = 1;

	forma[2][1][0] = 1;
	forma[2][1][1] = 1;
	forma[2][2][1] = 1;
	forma[2][2][2] = 1;
	
	forma[3][0][2] = 1;
	forma[3][1][1] = 1;
	forma[3][1][2] = 1;
	forma[3][2][1] = 1;
	
}else if (tipo == J) {
	
	forma[0][0][0] = 1;
	forma[0][1][0] = 1;
	forma[0][1][1] = 1;
	forma[0][1][2] = 1;
	
	forma[1][0][1] = 1;
	forma[1][0][2] = 1;
	forma[1][1][1] = 1;
	forma[1][2][1] = 1;
	
	forma[2][1][0] = 1;
	forma[2][1][1] = 1;
	forma[2][1][2] = 1;
	forma[2][2][2] = 1;

	forma[3][0][1] = 1;
	forma[3][1][1] = 1;
	forma[3][2][0] = 1;
	forma[3][2][1] = 1;
	
}else if (tipo == L) {
	
	forma[0][0][2] = 1;
	forma[0][1][0] = 1;
	forma[0][1][1] = 1;
	forma[0][1][2] = 1;
	
	forma[1][0][1] = 1;
	forma[1][1][1] = 1;
	forma[1][2][1] = 1;
	forma[1][2][2] = 1;
	
	forma[2][1][0] = 1;
	forma[2][1][1] = 1;
	forma[2][1][2] = 1;
	forma[2][2][0] = 1;
	
	forma[3][0][0] = 1;
	forma[3][0][1] = 1;
	forma[3][1][1] = 1;
	forma[3][2][1] = 1;
	
}
}

