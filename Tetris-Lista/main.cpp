#include <cstdlib>
#include <ctime>
#include "Interfaz.h"

int main() {
    srand((unsigned)time(NULL));     // las bolsas de piezas usan rand()
    Interfaz app;
    app.ejecutar();
    return 0;
}
