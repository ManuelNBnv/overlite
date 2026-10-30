#undef NDEBUG
#include "logic.h"
#include <cassert>
#include <cstdio>
#include <filesystem>
using namespace overlite;
int main() {
  std::filesystem::remove_all("DATA");
  // calculadora
  assert(calcular(Op::Suma, 2, 3).value == 5);
  assert(calcular(Op::Resta, 2, 3).value == -1);
  assert(calcular(Op::Multiplicacion, 4, 2.5).value == 10);
  assert(calcular(Op::Division, 1, 4).value == 0.25);
  assert(!calcular(Op::Division, 1, 0).ok);
  assert(!calcular(Op::Multiplicacion, 1e308, 10).ok);
  double d;
  assert(parseNumero("1,5", d) && d == 1.5);
  assert(parseNumero("-3", d) && d == -3);
  assert(!parseNumero("", d) && !parseNumero("1.2.3", d) && !parseNumero("abc", d) && !parseNumero("12x", d));
  assert(formatearNumero(12) == "12" && formatearNumero(0.5) == "0.5");
  // luhn (numeros de prueba publicos)
  assert(luhnValido("4111 1111 1111 1111"));
  assert(luhnValido("4111-1111-1111-1111"));
  assert(!luhnValido("4111 1111 1111 1112"));
  assert(!luhnValido("1234"));
  assert(!luhnValido("4111 1111 1111 111a"));
  // password
  for (int n : {0, 4, 16, 128, 1000}) {
    auto p = generarPassword(n);
    int esperado = n < 4 ? 4 : (n > 128 ? 128 : n);
    assert((int)p.size() == esperado);
  }
  assert(generarPassword(32) != generarPassword(32));
  // promedio
  assert(!promedio({}, d));
  assert(promedio({3, 4, 5}, d) && d == 4);
  // usuarios
  assert(!iniciarSesion("ana", "x"));
  assert(registrarUsuario("", "x") == RegistroResult::CamposVacios);
  assert(registrarUsuario("a\"b", "x") == RegistroResult::CaracteresInvalidos);
  assert(registrarUsuario("  ana  ", "clave 1") == RegistroResult::Ok);
  assert(registrarUsuario("ana", "otra") == RegistroResult::YaExiste);
  assert(registrarUsuario("luis", "pw") == RegistroResult::Ok);
  assert(iniciarSesion("ana", "clave 1"));
  assert(iniciarSesion(" luis ", "pw"));
  assert(!iniciarSesion("ana", "pw"));
  assert(!iniciarSesion("ana", "clave"));
  assert(!iniciarSesion("nadie", "pw"));
  std::filesystem::remove_all("DATA");
  puts("logic OK");
}
