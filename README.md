# Tetris con estructuras de datos lineales

**EIF207 – Estructuras de Datos · Proyecto I**
Universidad Nacional de Costa Rica, Sede Regional Brunca

**Autor:** Jorge Fernández Miranda · **Cédula:** 604770610
**Repositorio:** https://github.com/jorgefernandez667/Tetris\_Estructuras

Versión simplificada de Tetris en C++ con interfaz gráfica en **SFML**. Cada mecánica del juego se resuelve con una estructura de datos propia (sin `std::stack`, `std::queue`, `std::deque`, `std::list`, `std::priority\_queue` ni `std::sort`).

\---

## 1\. Requisitos

|Componente|Detalle|
|-|-|
|Lenguaje|C++11 o superior (se usan `nullptr` y `= delete`)|
|IDE / compilador|ZinjaI (MinGW) en Windows|
|Librería gráfica|**SFML 2.x** (módulos `graphics`, `window`, `system`)|
|Versiones probadas|SFML 2.4.2 de 32 bits en ZinjaI/Windows; SFML 2.6.1 con g++ 13 en Linux (compila sin advertencias con `-Wall`)|
|Fuente|`fuente.ttf` junto al ejecutable|

> No es compatible con SFML 3: el código usa la API de eventos y de `sf::VideoMode` de la serie 2.x.

Si `fuente.ttf` no está, el programa busca Arial, Segoe UI o Tahoma en `C:/Windows/Fonts`. Si no encuentra ninguna, el juego corre pero sin texto y muestra una barra roja en el menú.

\---

## 2\. Cómo compilar y ejecutar

### En ZinjaI (recomendado)

1. Abrir `Tetris-Lista/Tetris-Listas.zpr` (todo el código vive en la carpeta `Tetris-Lista/`).
2. Elegir la configuración `Debug\_Win32` o `Release\_Win32`. Ambas definen `SFML\_STATIC` y enlazan contra el complemento `sfml2` de ZinjaI (`${MINGW\_DIR}\\sfml2\\include` y `\\lib`).
3. Compilar y ejecutar con **F9**.
4. Verificar que `fuente.ttf` quede en el directorio de trabajo del ejecutable.

`puntajes.txt` se crea solo, en el directorio de trabajo, la primera vez que un jugador entra al top 10.



### Reproducir el benchmark de ordenamiento

El programa de medición está en `Tetris-Lista/Prueba/main\_Prueba.cpp` y **no** forma parte del `.zpr` (es un programa aparte, sin interfaz). Compílelo junto con `Ordenamiento.cpp` y ejecútelo desde `Tetris-Lista/`:

```bash
cd Tetris-Lista
g++ -std=c++11 -O2 -I. Prueba/main\_Prueba.cpp Ordenamiento.cpp -o benchmark
./benchmark
```

Ordena 10, 100, 1 000 y 10 000 registros aleatorios (semilla fija) con ambos algoritmos, verifica que den el mismo resultado y escribe `benchmark\_ordenamiento.csv`. En ZinjaI se puede crear un proyecto o configuración aparte con esos dos archivos.

\---

## 3\. Controles

**Menú:** `Enter` jugar · `P` mejores puntajes · `Esc` salir

**Durante la partida**

|Tecla|Acción|
|-|-|
|`←` `→` / `A` `D`|Mover (se repite al mantener)|
|`↑` / `W`|Rotar (4 orientaciones fijas, sin *wall kick*)|
|`↓` / `S`|Bajar una fila (se repite al mantener)|
|`Espacio`|Caída dura|
|`C`|Hold (una vez por pieza)|
|`Z` / `Y`|Deshacer / rehacer (con `Shift`, 5 pasos)|
|`P` / `Esc`|Pausa (`M` abandona y vuelve al menú)|

**Fin de partida:** si el puntaje entra al top 10 se pide el nombre (`Enter` guarda, `Esc` no guarda). Luego: `R` replay · `N` nueva partida · `P` puntajes · `M` menú.

**Replay:** botones en pantalla (`|<`, `<`, Play/Pausa, `>`, `>|`, `Vel -`, `Vel +`, `Salir`) o teclado: `←` `→` paso a paso · `Espacio` play/pausa · `Inicio` / `Fin` extremos · `↑` `↓` velocidad · `Esc` salir. Hay 4 velocidades (700, 350, 150 y 60 ms por paso).

**Mejores puntajes:** `A` alterna el algoritmo de ordenamiento y vuelve a ordenar la tabla; `M`, `Esc` o `Enter` regresan al menú.

\---

## 4\. Qué estructura resuelve cada mecánica

|Mecánica|Estructura propia|Archivos|
|-|-|-|
|Piezas futuras (bolsas de 7)|Cola circular con arreglo (16 casillas)|`ColaPiezas.h/.cpp`|
|Pieza en espera (hold)|Pila de capacidad 1|`PilaHold.h/.cpp`|
|Deshacer / rehacer / replay|Lista doblemente enlazada con cursor|`Replay.h/.cpp`|
|Eventos programados|Cola ordenada por momento (lista enlazada, inserción ordenada)|`ColaEventos.h/.cpp`|
|Tablero|Lista enlazada de 20 nodos, cada uno una fila de 10 celdas|`Tablero.h/.cpp`|
|Tabla de puntajes|Arreglo + inserción y mergesort propios|`Ordenamiento.h/.cpp`, `Puntajes.h/.cpp`|

### Organización de archivos

|Módulo|Responsabilidad|
|-|-|
|`Constantes.h`|Dimensiones del tablero, número de piezas, rotaciones y bloques|
|`Pieza.h/.cpp`|Tabla `FORMAS` con las 4 orientaciones precalculadas de las 7 piezas|
|`Estado.h`|Foto completa de la partida (lo que guarda cada nodo del replay)|
|`Juego.h/.cpp`|Reglas: caída, colisión, puntaje, eventos, hold, deshacer/rehacer. **No depende de SFML**|
|`Interfaz.h/.cpp`|Ventana SFML, pantallas, animaciones, entrada de teclado y ratón|
|`main.cpp`|Inicializa `rand()` y lanza la interfaz|
|`BotPrueba.h`|Jugador automático usado solo en pruebas: elige dónde colocar cada pieza y la coloca con las acciones reales|
|`Prueba/main\_Prueba.cpp`|Programa de medición del ordenamiento (genera `benchmark\_ordenamiento.csv`)|
|`puntajes.txt`|Tabla de mejores puntajes (archivo de ejemplo generado por el propio programa)|

La lógica del juego está separada de la interfaz: `Juego` puede probarse sin abrir ventana.

\---

## 5\. Reglas y decisiones de diseño

**Piezas.** Cada bolsa se mezcla con Fisher-Yates. Cuando quedan menos de 7 piezas en la cola se agrega otra bolsa, así que siempre hay al menos 7 disponibles y la interfaz muestra las 3 siguientes.

**Rotación.** Cuatro orientaciones escritas a mano por pieza. Si la orientación siguiente no cabe, la pieza no rota.

**Fin de juego.** Ocurre cuando una pieza nueva colisiona en su posición inicial.

**Puntaje.** Por líneas limpiadas de una vez:

|Líneas|1|2|3|4|
|-|-|-|-|-|
|Puntos|100|300|500|800|

**Eventos programados.** Los eventos se miden en piezas colocadas y se reprograman solos después de dispararse:

|Evento|Primera vez|Se repite cada|Efecto|
|-|-|-|-|
|Acelerar la caída|pieza 10|10 piezas|Sube el nivel (máx. 10). Intervalo de caída: `800 − (nivel−1)·70` ms, mínimo 100 ms|
|Pieza especial|pieza 7|12 piezas|La próxima pieza es dorada; al colocarse elimina la fila de su bloque más bajo. No se puede enviar al hold|
|Puntos dobles|pieza 15|20 piezas|Las siguientes 5 piezas dan el doble de puntos|

Los eventos se insertan siempre en su posición correcta (nunca al final para reordenar después). En un empate de momento se respeta el orden de llegada. El panel muestra el próximo evento y cuántas piezas faltan.

**Hold.** Se puede usar una vez por pieza. Si el hold está vacío guarda la actual y saca la siguiente de la cola; si no, intercambia.

**Historial.** Cada acción (mover, rotar, bajar, colocar, hold) agrega un nodo con una foto completa del estado (tablero, pieza, hold, cola, puntaje y eventos). Se registra la caída dura como **un solo paso**. Al deshacer varios pasos y hacer una acción nueva, los nodos "futuros" se descartan. El replay al terminar la partida recorre la lista desde el primer nodo.

**Persistencia.** `puntajes.txt` guarda una línea por registro con el formato `puntaje nombre`. Solo entran puntajes mayores a 0 y, con la tabla llena, deben superar estrictamente al décimo lugar. Al cargar, las líneas mal formadas se ignoran y el archivo no necesita venir ordenado. Los nombres admiten letras, números, espacio y guion, hasta 19 caracteres; si queda vacío se guarda como `Anonimo`.

**Memoria.** Cada estructura dinámica libera todos sus nodos en el destructor, y `Tablero`, `Replay` y `ColaEventos` prohíben la copia (`= delete`). En cada eliminación se reconecta la lista antes de hacer `delete`.

\---

## 6\. Complejidad de las operaciones principales

|Operación|Costo|
|-|-|
|Cola de piezas: encolar / desencolar / ver|O(1) (desencolar es O(1) amortizado por la recarga de bolsa)|
|Pila hold: push / pop / top|O(1)|
|Cola de eventos: insertar ordenado|O(n) en el peor caso, O(1) si va al frente (n ≤ 3)|
|Cola de eventos: ver frente / desencolar|O(1)|
|Tablero: acceder a la fila *f*|O(f)|
|Tablero: insertar fila vacía|O(1) (más inicializar 10 celdas)|
|Tablero: eliminar una fila|O(f) para llegar al nodo, O(1) para quitarlo|
|Tablero: limpiar líneas|O(FILAS · COLUMNAS) para revisar, O(1) por fila quitada e insertada|
|Replay: registrar|O(1) (más descartar los nodos futuros, si los hay)|
|Replay: retroceder / avanzar 1 paso|O(1)|
|Replay: retroceder / avanzar *k* pasos|O(k)|
|Ordenar la tabla: inserción|O(n²)|
|Ordenar la tabla: mergesort|O(n log n)|

### Comparación empírica de ordenamiento

Tiempos medidos (µs) en `benchmark\_ordenamiento.csv`:

|n|Inserción|Mergesort|
|-|-|-|
|10|0,23|0,59|
|100|13,88|8,38|
|1 000|1 250,41|178,37|
|10 000|120 018|2 382,94|

Con 10 registros la inserción gana (mergesort paga la memoria auxiliar); desde 100 ya pierde, y con 10 000 es unas 50 veces más lenta. Un análisis más completo se encuentra en el informe.

\---

## 7\. Limitaciones conocidas

* No hay *wall kick*: una rotación que no cabe simplemente se ignora (así lo pide el enunciado).
* Cada nodo del historial guarda una foto completa de aproximadamente 1 KB, por lo que una partida muy larga consume más memoria que un historial de acciones.
* El deshacer/rehacer solo funciona mientras la partida está en curso; después del game over el historial se usa únicamente para el replay.
* Solo hay una partida a la vez; no hay guardado de partidas a medias.

\---

## 8\. Control de versiones

El proyecto se desarrolló con git y commits incrementales, un módulo o etapa por commit (piezas, colas, pila, tablero, juego, eventos, replay, ordenamiento y puntajes, interfaz). Los prototipos de consola que se usaron para probar cada etapa quedaron en versiones anteriores de `main.cpp` dentro del historial.

