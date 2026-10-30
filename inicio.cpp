#include "funciones.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

int logeo();
int registro();
int salir();

int inicio() {

  int login;

  imprimir("---------------------------------");
  imprimir("BIENVENIDO AL LOGIN DE OVERLITE");
  imprimir("---------------------------------");

  imprimir("Deseas iniciar seccion (1), registrarte (2) o salir (3) ?");
  cin >> login;

  switch (login) {

  case 1:
    logeo();
    break;

  case 2:
    registro();
    break;
  case 3:
    salir();
    break;
  }
  return 0;
}

int logeo() {
  system("clear");
  string user, pass;
  string linea;

  imprimir("--------------------------------------------");
  imprimir("BIENVENIDO AL INICIO DE SECCION DE OVERLITE");

  imprimir("------------------------------------------");
  imprimir("Usuario:");
  cin >> user;

  imprimir("------------------------------------------");
  imprimir("Contraseña:");
  imprimir("------------------------------------------");
  cin >> pass;

  ifstream archivo("DATA/users.json");

  while (getline(archivo, linea)) {

    size_t u1 = linea.find("\"user\":\"");
    size_t p1 = linea.find("\"pass\":\"");

    if (u1 == string::npos || p1 == string::npos) {
      continue;
    }

    u1 += 8;
    size_t u2 = linea.find("\"", u1);

    p1 += 8;
    size_t p2 = linea.find("\"", p1);

    string file_user = linea.substr(u1, u2 - u1);
    string file_pass = linea.substr(p1, p2 - p1);

    if (user == file_user && pass == file_pass) {
      imprimir("------------------------------------------");
      imprimir("Login correcto");
      imprimir("------------------------------------------");
      imprimir("ya inicia el programa");
      imprimir("------------------------------------------");

      sleep(3);

      system("clear");

      return Programa();
    }
  }
  imprimir("Usuario o contraseña incorrectos");
  archivo.close();
  return main();
}

int registro() {
  system("clear");

  string user, pass;
  imprimir("------------------------------------------");
  imprimir("BIENVENIDO AL REGISTRO DE USUARIOS NUEVOS");

  imprimir("---------------------------------");
  imprimir("Nuevo usuario:");
  cin >> user;

  imprimir("---------------------------------");
  imprimir("Nueva contraseña:");
  imprimir("---------------------------------");
  cin >> pass;

  ofstream archivo("DATA/users.json", ios::app);

  archivo << "{\"user\":\"" << user << "\",\"pass\":\"" << pass << "\"}"
          << endl;

  archivo.close();

  imprimir("Usuario registrado correctamente");
  imprimir("Inicia seccion con tu nuevo usuario");
  imprimir("---------------------------------");

  sleep(3);

  system("clear");

  return main();
}

int salir() { return 0; }
