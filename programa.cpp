#include "funciones.h"
#include <iostream>

using namespace std;

int Programa() {

  system("clear");

  int respuesta;

  imprimir("--------------------------");
  imprimir("BIENVENIDO A OVERLITE");
  imprimir("--------------------------");
  imprimir("Que quieres hacer?");

  imprimir("--------------------------");
  imprimir("1. Calculadora");
  imprimir("2. Generador de contraseñas");
  imprimir("3. validador de tarjetas");
  imprimir("4. Calculo de notas");
  imprimir("5. Cerrar el programa");
  imprimir("--------------------------");

  cin >> respuesta;

  switch (respuesta) {
  case 1:
    Calculadora();
    break;
  case 2:
    Generador();
    break;
  case 3:
    Validador();
    break;
  case 4:
    Calculo();
    break;
  case 5:
    system("clear");
    return 0;
  default:
    imprimir("Opcion no valida");
    sleep(2);
    system("clear");
    return 0;
  }

  system("clear");
  return 0;
}
