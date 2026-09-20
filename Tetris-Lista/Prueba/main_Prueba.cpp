#include <SFML/Graphics.hpp>
using namespace sf;

#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "Ordenamiento.h"
using namespace std;

typedef chrono::steady_clock Reloj;

double segundosDesde(Reloj::time_point t0) {
	return chrono::duration<double>(Reloj::now() - t0).count();
}

// Tiempo medio, en microsegundos, de UN ordenamiento de los n registros de 'base'.
// Se repite hasta acumular al menos 0.25 s (asi el reloj de Windows, que es grueso, no molesta)
// y se resta el tiempo de las copias, que hay que hacer para reordenar siempre datos desordenados.
double microsPorOrdenamiento(int algoritmo, const Registro base[], int n) {
	Registro* trabajo = new Registro[n];
	long repeticiones = 1;
	double total = 0;
	while (true) {
		Reloj::time_point t0 = Reloj::now();
		for (long r = 0; r < repeticiones; r++) {
			copiarRegistros(trabajo, base, n);
			ordenar(trabajo, n, algoritmo);
		}
		total = segundosDesde(t0);
		if (total >= 0.25 || repeticiones >= (1L << 26)) break;
		repeticiones *= 2;
	}
	Reloj::time_point t1 = Reloj::now();
	for (long r = 0; r < repeticiones; r++) copiarRegistros(trabajo, base, n);
	double soloCopias = segundosDesde(t1);
	delete[] trabajo;
	double neto = total - soloCopias;
	if (neto < 0) neto = 0;
	return neto / repeticiones * 1e6;
}

int main() {
	srand(12345);                                    // semilla fija: mismos datos en cada corrida
	const int TAMANOS[] = { 10, 100, 1000, 10000 };
	const int CANTIDAD = 4;
	
	ofstream csv("benchmark_ordenamiento.csv");
	csv << "n,insercion_us,merge_us\n";
	
	cout << "Ordenar n registros aleatorios de mayor a menor (tiempo por ordenamiento)\n\n";
	cout << setw(8) << "n" << setw(18) << "insercion (us)" << setw(16) << "merge (us)"
		<< setw(17) << "insercion/merge" << setw(20) << "x insercion vs n/10" << setw(16) << "x merge vs n/10" << "\n";
	cout << string(95, '-') << "\n";
	
	double antesIns = 0, antesMer = 0;
	for (int t = 0; t < CANTIDAD; t++) {
		int n = TAMANOS[t];
		Registro* base = new Registro[n];
		for (int i = 0; i < n; i++) { copiarNombre(base[i], "Jugador"); base[i].puntaje = 1 + rand() % 100000; }
		
		// Comprobar primero que los dos dan el mismo resultado y estan ordenados
		Registro* a = new Registro[n];
		Registro* b = new Registro[n];
		copiarRegistros(a, base, n); copiarRegistros(b, base, n);
		ordenarInsercion(a, n); ordenarMerge(b, n);
		bool iguales = estaOrdenado(a, n) && estaOrdenado(b, n);
		for (int i = 0; i < n; i++) if (a[i].puntaje != b[i].puntaje) iguales = false;
		if (!iguales) { cout << "ERROR: los algoritmos no coinciden para n=" << n << "\n"; return 1; }
		delete[] a; delete[] b;
		
		double ins = microsPorOrdenamiento(ALG_INSERCION, base, n);
		double mer = microsPorOrdenamiento(ALG_MERGE, base, n);
		delete[] base;
		
		cout << setw(8) << n << fixed << setprecision(2) << setw(18) << ins << setw(16) << mer
			<< setw(17) << (mer > 0 ? ins / mer : 0.0);
		if (t > 0) cout << setw(20) << (antesIns > 0 ? ins / antesIns : 0.0) << setw(16) << (antesMer > 0 ? mer / antesMer : 0.0);
		cout << "\n";
		csv << n << "," << ins << "," << mer << "\n";
		antesIns = ins; antesMer = mer;
	}
	csv.close();
	
	cout << "\nPrediccion teorica al multiplicar n por 10:  insercion x100 (n^2),  merge poco mas de x10 (n log n).\n";
	cout << "Datos guardados en benchmark_ordenamiento.csv\n";
	return 0;
}
