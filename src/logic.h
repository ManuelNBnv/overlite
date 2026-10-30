// Lógica de Overlite, sin nada de interfaz: se puede probar sin abrir ninguna ventana.
#pragma once

#include <string>
#include <vector>

namespace overlite {

// ---------- Calculadora ----------
enum class Op { Suma, Resta, Multiplicacion, Division };

struct CalcResult {
  bool ok = false;
  double value = 0;
  std::string error; // se llena cuando ok == false
};

CalcResult calcular(Op op, double a, double b);

// Convierte texto a número. Acepta coma o punto decimal. false si no es un número válido.
bool parseNumero(const std::string &texto, double &out);

// Formatea sin ceros sobrantes (p. ej. 12, 0.5, 3.333333333).
std::string formatearNumero(double v);

// ---------- Validador de tarjetas ----------
// Solo comprueba el dígito de control (algoritmo de Luhn) y la longitud (13-19 dígitos).
// No confirma que la tarjeta exista ni que tenga fondos.
bool luhnValido(const std::string &tarjeta);

// ---------- Generador de contraseñas ----------
constexpr int kPassMin = 4;
constexpr int kPassMax = 128;
std::string generarPassword(int longitud); // longitud se ajusta a [kPassMin, kPassMax]

// ---------- Notas ----------
bool promedio(const std::vector<double> &notas, double &out);

// ---------- Usuarios (DATA/users.json, una línea JSON por usuario) ----------
enum class RegistroResult { Ok, CamposVacios, CaracteresInvalidos, YaExiste, ErrorArchivo };

// Quita espacios al inicio y al final del nombre de usuario (así se guarda y se compara).
std::string normalizarUsuario(const std::string &user);

RegistroResult registrarUsuario(const std::string &user, const std::string &pass);
bool iniciarSesion(const std::string &user, const std::string &pass);

} // namespace overlite
