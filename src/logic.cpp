#include "logic.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <random>

namespace overlite {

// ---------------------------------------------------------------- Calculadora

CalcResult calcular(Op op, double a, double b) {
  CalcResult r;
  switch (op) {
  case Op::Suma:
    r.value = a + b;
    break;
  case Op::Resta:
    r.value = a - b;
    break;
  case Op::Multiplicacion:
    r.value = a * b;
    break;
  case Op::Division:
    if (b == 0) {
      r.error = "No se puede dividir entre cero";
      return r;
    }
    r.value = a / b;
    break;
  }
  if (!std::isfinite(r.value)) {
    r.error = "El resultado está fuera de rango";
    return r;
  }
  r.ok = true;
  return r;
}

bool parseNumero(const std::string &texto, double &out) {
  std::string s = texto;
  std::replace(s.begin(), s.end(), ',', '.');
  if (s.empty())
    return false;
  char *fin = nullptr;
  double v = std::strtod(s.c_str(), &fin);
  if (fin == s.c_str() || *fin != '\0' || !std::isfinite(v))
    return false;
  out = v;
  return true;
}

std::string formatearNumero(double v) {
  char buf[64];
  std::snprintf(buf, sizeof buf, "%.10g", v);
  return buf;
}

// ---------------------------------------------------------------- Luhn

bool luhnValido(const std::string &tarjeta) {
  std::string numero;
  for (char c : tarjeta) {
    if (std::isdigit(static_cast<unsigned char>(c))) {
      numero += c;
    } else if (c != ' ' && c != '-') {
      return false;
    }
  }

  if (numero.size() < 13 || numero.size() > 19)
    return false;

  int suma = 0;
  bool duplicar = false;
  for (int i = static_cast<int>(numero.size()) - 1; i >= 0; --i) {
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

// ---------------------------------------------------------------- Contraseñas

std::string generarPassword(int longitud) {
  static const std::string caracteres = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789"
                                        "!@#$%&*+-_?";
  longitud = std::clamp(longitud, kPassMin, kPassMax);

  // Se usa random_device directamente (no un mt19937 sembrado con 32 bits):
  // para contraseñas conviene que cada carácter salga de la fuente del sistema.
  std::random_device rd;
  std::uniform_int_distribution<std::size_t> dist(0, caracteres.size() - 1);

  std::string out;
  out.reserve(longitud);
  for (int i = 0; i < longitud; ++i)
    out += caracteres[dist(rd)];
  return out;
}

// ---------------------------------------------------------------- Notas

bool promedio(const std::vector<double> &notas, double &out) {
  if (notas.empty())
    return false;
  double suma = 0;
  for (double n : notas)
    suma += n;
  out = suma / static_cast<double>(notas.size());
  return true;
}

// ---------------------------------------------------------------- Usuarios

namespace {

const char *kUsersPath = "DATA/users.json";

std::string recortar(const std::string &s) {
  size_t a = 0, b = s.size();
  while (a < b && std::isspace(static_cast<unsigned char>(s[a])))
    ++a;
  while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1])))
    --b;
  return s.substr(a, b - a);
}

// El formato es de una línea por usuario y sin escapes, así que se prohíben
// las comillas, la barra invertida y los caracteres de control.
bool textoSeguro(const std::string &s) {
  for (unsigned char c : s) {
    if (c < 0x20 || c == 0x7f || c == '"' || c == '\\')
      return false;
  }
  return true;
}

// Lee {"user":"x","pass":"y"} con el mismo formato que usaba la versión de consola.
bool leerLinea(const std::string &linea, std::string &user, std::string &pass) {
  const std::string ku = "\"user\":\"";
  const std::string kp = "\"pass\":\"";

  size_t u1 = linea.find(ku);
  size_t p1 = linea.find(kp);
  if (u1 == std::string::npos || p1 == std::string::npos)
    return false;

  u1 += ku.size();
  size_t u2 = linea.find('"', u1);
  p1 += kp.size();
  size_t p2 = linea.find('"', p1);
  if (u2 == std::string::npos || p2 == std::string::npos)
    return false;

  user = linea.substr(u1, u2 - u1);
  pass = linea.substr(p1, p2 - p1);
  return true;
}

} // namespace

std::string normalizarUsuario(const std::string &user) { return recortar(user); }

RegistroResult registrarUsuario(const std::string &userIn, const std::string &pass) {
  const std::string user = recortar(userIn);
  if (user.empty() || pass.empty())
    return RegistroResult::CamposVacios;
  if (!textoSeguro(user) || !textoSeguro(pass))
    return RegistroResult::CaracteresInvalidos;

  {
    std::ifstream in(kUsersPath);
    std::string linea, u, p;
    while (std::getline(in, linea)) {
      if (leerLinea(linea, u, p) && u == user)
        return RegistroResult::YaExiste;
    }
  }

  std::error_code ec;
  std::filesystem::create_directories("DATA", ec);

  std::ofstream out(kUsersPath, std::ios::app);
  if (!out)
    return RegistroResult::ErrorArchivo;
  out << "{\"user\":\"" << user << "\",\"pass\":\"" << pass << "\"}\n";
  out.flush();
  return out ? RegistroResult::Ok : RegistroResult::ErrorArchivo;
}

bool iniciarSesion(const std::string &userIn, const std::string &pass) {
  const std::string user = recortar(userIn);
  if (user.empty() || pass.empty())
    return false;

  std::ifstream in(kUsersPath);
  std::string linea, u, p;
  while (std::getline(in, linea)) {
    if (leerLinea(linea, u, p) && u == user && p == pass)
      return true;
  }
  return false;
}

} // namespace overlite
