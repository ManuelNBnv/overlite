#include "funciones.h"
#include <iomanip>
#include <iostream>

using namespace std;

int Calculo() {

  system("clear");

  int cantidad;
  double nota;
  double suma = 0;

  imprimir("-----------------------------------------------");
  imprimir("HOLA BIENVENIDO AL SISTEMA DE CALCULO DE NOTAS");
  imprimir("-----------------------------------------------");

  imprimir("Digita la cantidad de notas que desea digitar: ");
  imprimir("-----------------------------------------------");

  cin >> cantidad;

  for (int i = 1; i <= cantidad; i++) {
    cout << "Digite la nota " << i << endl;
    cin >> nota;
    suma += nota;
  }

  imprimir("el promedio del estudiante es de:");
  imprimir(nota);

  int fin;

  cout << fixed << setprecision(2);
  imprimir("-----------------------------------------------");
  imprimir("\nQuieres volver al inicio o terminar?");
  imprimir("1. volver \n2. terminar");

  cin >> fin;

  switch (fin) {
  case 1:
    return main();
  case 2:
    return 0;
  default:
    imprimir("Opcion no valida te terminaremos el programa");
    sleep(2);
    system("clear");
  }

  return 0;
}
