#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <iomanip>
#include <string>
using namespace std;

typedef vector< vector<char> > Matriz;

int pedirNumero(const string& mensaje, int minimo, int maximo) {
    int numero;
    while (true) {
        cout << mensaje;
        if (cin >> numero && numero >= minimo && numero <= maximo) return numero;
        cout << "Error: escribe un numero entre " << minimo << " y " << maximo << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int pedirFila() {
    int fila;
    while (true) {
        cout << "Fila (0 = salir, -1 = ver minas): ";
        if (cin >> fila) return fila;
        cout << "Error: escribe un numero entero.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void pedirTamano(int& filas, int& columnas) {
    filas = pedirNumero("Numero de filas (3 a 9): ", 3, 9);
    columnas = pedirNumero("Numero de columnas (3 a 9): ", 3, 9);
}

Matriz crearMatriz(int filas, int columnas, char simbolo) {
    return Matriz(filas, vector<char>(columnas, simbolo));
}

void colocarMinas(Matriz& minas, int filas, int columnas) {
    int colocadas = 0;
    while (colocadas < 3) {
        int fila = rand() % filas;
        int columna = rand() % columnas;
        if (minas[fila][columna] != '*') {
            minas[fila][columna] = '*';
            colocadas++;
        }
    }
}

void mostrarTablero(const Matriz& tablero) {
    cout << "\n   ";
    for (size_t c = 0; c < tablero[0].size(); ++c) cout << c + 1 << ' ';
    cout << "\n";
    for (size_t f = 0; f < tablero.size(); ++f) {
        cout << setw(2) << f + 1 << ' ';
        for (size_t c = 0; c < tablero[f].size(); ++c) cout << tablero[f][c] << ' ';
        cout << "\n";
    }
    cout << "\n";
}

void mostrarMinas(const Matriz& minas) {
    cout << "Ubicacion de las minas (modo prueba):\n";
    mostrarTablero(minas);
}

void revelarMinas(Matriz& tablero, const Matriz& minas) {
    for (size_t f = 0; f < minas.size(); ++f)
        for (size_t c = 0; c < minas[f].size(); ++c)
            if (minas[f][c] == '*') tablero[f][c] = '*';
}

bool abrirCasilla(Matriz& tablero, const Matriz& minas, int fila, int columna) {
    if (minas[fila][columna] == '*') {
        tablero[fila][columna] = '*';
        return false;
    }
    tablero[fila][columna] = '+';
    return true;
}

bool hayVictoria(int abiertas, int filas, int columnas) {
    return abiertas == filas * columnas - 3;
}

void guardarHistorial(int filas, int columnas, const string& resultado, int intentos) {
    ofstream archivo("historial.txt", ios::app);
    if (!archivo) {
        cout << "Aviso: no se pudo guardar el historial.\n";
        return;
    }
    archivo << filas << ' ' << columnas << ' ' << resultado << ' ' << intentos << '\n';
}

void mostrarResumen() {
    ifstream archivo("historial.txt");
    int filas, columnas, intentos, ganadas = 0, perdidas = 0;
    string resultado;
    while (archivo >> filas >> columnas >> resultado >> intentos) {
        if (resultado == "gano") ganadas++;
        else if (resultado == "perdio") perdidas++;
    }
    cout << "Partidas: " << ganadas + perdidas
         << " | Ganadas: " << ganadas
         << " | Perdidas: " << perdidas << "\n\n";
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    cout << "========== BUSCAMINAS ==========\n";
    mostrarResumen();

    int filas, columnas;
    pedirTamano(filas, columnas);
    Matriz minas = crearMatriz(filas, columnas, '.');
    Matriz tablero = crearMatriz(filas, columnas, '?');
    colocarMinas(minas, filas, columnas);

    int intentos = 0;
    int abiertas = 0;
    while (true) {
        mostrarTablero(tablero);
        int fila = pedirFila();
        if (fila == 0) {
            cout << "Saliste del juego. No se guarda esta partida.\n";
            break;
        }
        if (fila == -1) {
            mostrarMinas(minas);
            continue;
        }
        if (fila < 1 || fila > filas) {
            cout << "Error: fila fuera del tablero.\n";
            continue;
        }
        int columna = pedirNumero("Columna: ", 1, columnas);
        if (tablero[fila - 1][columna - 1] != '?') {
            cout << "Esa casilla ya estaba abierta. No cuenta como intento.\n";
            continue;
        }

        intentos++;
        if (!abrirCasilla(tablero, minas, fila - 1, columna - 1)) {
            cout << "Pisaste una mina. Perdiste.\n";
            revelarMinas(tablero, minas);
            mostrarTablero(tablero);
            guardarHistorial(filas, columnas, "perdio", intentos);
            break;
        }
        abiertas++;
        cout << "Casilla segura (+). Intentos: " << intentos << "\n";
        if (hayVictoria(abiertas, filas, columnas)) {
            cout << "Ganaste! Abriste todas las casillas sin minas.\n";
            mostrarTablero(tablero);
            guardarHistorial(filas, columnas, "gano", intentos);
            break;
        }
    }
    cout << "\nResumen actualizado:\n";
    mostrarResumen();
    cout << "Presiona ENTER para finalizar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
    return 0;
}
