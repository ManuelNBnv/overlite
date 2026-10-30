#include "funciones.h"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;
// 5254133674403564
string tarjeta;

bool verificarTarjeta(const string &tarjeta) {
  string numero;

  for (char c : tarjeta) {
    if (isdigit(static_cast<unsigned char>(c))) {
      numero += c;
    } else if (c != ' ' && c != '-') {
      return false;
    }
  }

  if (numero.length() < 13 || numero.length() > 19)
    return false;

  int suma = 0;
  bool duplicar = false;

  for (int i = numero.length() - 1; i >= 0; i--) {
    int digito = numero[i] - '0';

    if (duplicar) {
      digito *= 2;
      if (digito > 9)
        digito -= 9;
    }

    suma += digito;
    duplicar = !duplicar;
  }

  return suma % 10 == 0;
}

int Validador() {

  system("clear");

  imprimir("------------------------------------------");
  imprimir("HOLA BIENVENIDO A EL VALIDADOR DE TARJETAS");
  imprimir("------------------------------------------");
  imprimir("Porfavor ingresa el numero de la tarjeta");
  imprimir("------------------------------------------");

  getline(cin >> ws, tarjeta);

  if (verificarTarjeta(tarjeta)) {
    imprimir("------------------------------------------");
    imprimir("Tarjeta valida y completamente funcional");
  } else {
    imprimir("------------------------------------------");
    imprimir("Tarjeta invalida");
  }

  int fin;

  imprimir("-----------------------------------------------");
  imprimir("\nQuieres volver al inicio o terminar?");
  imprimir("1. volver \n2. terminar");

  cin >> fin;

  switch (fin) {
  case 1:
    return Programa();

  case 2:
    return 0;
  default:
    imprimir("Opcion no valida te terminaremos el programa");
    sleep(2);
    system("clear");
  }

  return 0;
}
