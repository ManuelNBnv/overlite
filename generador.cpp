#include "funciones.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>

using namespace std;

int Generador() {
  system("clear");

  int longitud;

  imprimir("------------------------------------------");
  imprimir("HOLA BIENVENIDO A EL GENERADOR DE TARJETAS");
  imprimir("------------------------------------------");
  imprimir("Porfavor ingresa la cantidad de digitos que quieres");
  imprimir("------------------------------------------");

  cin >> longitud;

  system("clear");

  imprimir("-----------------------------------------------");
  imprimir("Tu contraseña es");
  imprimir("-----------------------------------------------");

  const string caracteres = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz"
                            "0123456789"
                            "!@#$%&*+-_?";

  static random_device rd;
  static mt19937 gen(rd());

  uniform_int_distribution<> dist(0, caracteres.length() - 1);

  for (int i = 0; i < longitud; i++) {
    cout << caracteres[dist(gen)];
  }

  cout << endl;

  int fin;

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
