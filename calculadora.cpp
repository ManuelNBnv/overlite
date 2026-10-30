#include "funciones.h"
#include <iostream>

using namespace std;

int Calculadora() {

  int op;

  system("clear");

  imprimir("------------------------------------");
  imprimir("HOLA BIENVENIDO A LA CALCULADORA");
  imprimir("------------------------------------");
  imprimir("Porfavor ingresa el tipo de operacion que deseas hacer");
  imprimir("------------------------------------");

  imprimir("1. Multiplicacion");
  imprimir("2. Suma");
  imprimir("3. Resta");
  imprimir("4. Divicion");
  imprimir("5. Volver");

  cin >> op;

  switch (op) {
  case 1:
    int numm[1];

    system("clear");

    imprimir("Ahora digita el primer numero");
    cin >> numm[0];
    imprimir("Ahora el segundo numero");
    cin >> numm[1];

    system("clear");

    imprimir("El resultado es: ");
    imprimir(numm[0] * numm[1]);
    break;

  case 2:
    int nums[1];

    system("clear");

    imprimir("Ahora digita el primer numero");
    cin >> nums[0];
    imprimir("Ahora el segundo numero");
    cin >> nums[1];

    system("clear");

    imprimir("El resultado es: ");
    imprimir(nums[0] + nums[1]);
    break;

  case 3:
    int numr[1];

    system("clear");

    imprimir("Ahora digita el primer numero");
    cin >> numr[0];
    imprimir("Ahora el segundo numero");
    cin >> numr[1];

    system("clear");

    imprimir("El resultado es: ");
    imprimir(numr[0] - numr[1]);
    break;

  case 4:
    float numd[1];

    system("clear");

    imprimir("Ahora digita el primer numero");
    cin >> numd[0];
    imprimir("Ahora el segundo numero");
    cin >> numd[1];

    system("clear");

    imprimir("El resultado es: ");
    imprimir(numd[0] / numd[1]);
    break;

  case 5:

    system("clear");
    return Programa();

    break;

  default:

    imprimir("Opcion no valida");

    break;

    return Calculadora();
  }

  int fin;

  imprimir("-----------------------------------------------");
  imprimir("\nQuieres volver al inicio o terminar?");
  imprimir("1. volver \n2. terminar");

  cin >> fin;

  switch (fin) {
  case 1:
    return Calculadora();
  case 2:
    system("clear");
    return 0;
  default:
    imprimir("Opcion no valida te terminaremos el programa");
    sleep(2);
    system("clear");
  }
  system("clear");
  return 0;
}
