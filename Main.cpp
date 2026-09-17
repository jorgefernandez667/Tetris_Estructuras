#include <iostream>
#include "juego/Pieza.h"

int main() {
	Pieza pieza(L);
	
	for (int i = 0; i < 4; i++) {
		std::cout << "\nOrientacion " << i << ":" << std::endl;
		
		pieza.mostrarForma();
		pieza.rotar();
	}
	
	return 0;
}
