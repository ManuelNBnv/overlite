#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

int Calculadora();
int Generador();
int Validador();
int Calculo();
int Programa();
int inicio();

template <typename T> void imprimir(T texto) { cout << texto << endl; }

int main();

#endif