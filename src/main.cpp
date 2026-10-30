// Overlite con interfaz gráfica: SDL3 + SDL3_ttf, todo dibujado a mano.
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include "logic.h"
#include "ui.h"

using namespace overlite;
using ui::Input;
using ui::Size;
using ui::Tone;

namespace {

constexpr int kW = 820;
constexpr int kH = 620;
const SDL_FRect kPanel{70, 120, 680, 470};
const SDL_FRect kBack{90, 530, 150, 44};

enum class Screen { Inicio, Login, Registro, Menu, Calculadora, Generador, Validador, Notas };

struct App {
  bool running = true;
  Screen screen = Screen::Inicio;
  Screen pending = Screen::Inicio;

  std::string currentUser;

  // Mensaje de estado de la pantalla actual
  std::string note;
  bool noteGood = false;

  // Login / registro
  std::string user, pass, pass2;

  // Calculadora
  std::string calcA, calcB, calcResult;

  // Generador
  std::string genLen = "16", genPass;

  // Validador
  std::string card;
  int cardState = 0; // 0 = sin resultado, 1 = pasa Luhn, 2 = no pasa

  // Notas
  std::string noteInput, avg;
  std::vector<double> notes;
};

// Cambia de pantalla al final del fotograma y limpia lo que se escribió
// (así una contraseña o un número de tarjeta no se quedan en memoria de pantallas viejas).
void go(App &a, Screen s, const std::string &msg = "", bool good = false) {
  a.pending = s;
  a.note = msg;
  a.noteGood = good;
  a.user.clear();
  a.pass.clear();
  a.pass2.clear();
  a.calcA.clear();
  a.calcB.clear();
  a.calcResult.clear();
  a.genLen = "16";
  a.genPass.clear();
  a.card.clear();
  a.cardState = 0;
  a.noteInput.clear();
  a.avg.clear();
  a.notes.clear();
}

void fail(App &a, const std::string &msg) {
  a.note = msg;
  a.noteGood = false;
}

void ok(App &a, const std::string &msg) {
  a.note = msg;
  a.noteGood = true;
}

void encabezado(ui::Ui &ui, const std::string &titulo, const std::string &sub = "",
                Tone tono = Tone::Normal) {
  ui.textCentered(titulo, kW / 2.0f, 24, Size::Title, tono);
  if (!sub.empty())
    ui.textCentered(sub, kW / 2.0f, 74, Size::Body, Tone::Muted);
  ui.accentLine(kW / 2.0f, 106, 90);
  ui.panel(kPanel);
}

void mostrarNota(ui::Ui &ui, const App &a, float y) {
  if (!a.note.empty())
    ui.textCentered(a.note, kW / 2.0f, y, Size::Body, a.noteGood ? Tone::Good : Tone::Bad);
}

// ------------------------------------------------------------------ pantallas

void pantallaInicio(ui::Ui &ui, App &a) {
  encabezado(ui, "OVERLITE", "Inicia sesión o crea una cuenta", Tone::Accent);

  if (ui.button("Iniciar sesión", {250, 190, 320, 52}, true))
    go(a, Screen::Login);
  if (ui.button("Registrarse", {250, 262, 320, 52}))
    go(a, Screen::Registro);
  if (ui.button("Salir", {250, 334, 320, 52}))
    a.running = false;

  mostrarNota(ui, a, 430);
}

void pantallaLogin(ui::Ui &ui, App &a) {
  encabezado(ui, "Iniciar sesión", "Entra con tu usuario de Overlite");

  ui.textField("Usuario", {160, 210, 500, 44}, a.user, Input::Text, false, 32);
  ui.textField("Contraseña", {160, 300, 500, 44}, a.pass, Input::Text, true, 64);

  const bool enviar = ui.button("Entrar", {160, 385, 500, 48}, true) || ui.enterPressed();
  mostrarNota(ui, a, 450);

  if (ui.button("Volver", kBack)) {
    go(a, Screen::Inicio);
    return;
  }

  if (enviar) {
    if (iniciarSesion(a.user, a.pass)) {
      const std::string nombre = normalizarUsuario(a.user);
      go(a, Screen::Menu);
      a.currentUser = nombre;
    } else {
      a.pass.clear();
      fail(a, "Usuario o contraseña incorrectos");
    }
  }
}

void pantallaRegistro(ui::Ui &ui, App &a) {
  encabezado(ui, "Crear cuenta", "Elige un usuario y una contraseña");

  ui.textField("Nuevo usuario", {160, 190, 500, 44}, a.user, Input::Text, false, 32);
  ui.textField("Nueva contraseña", {160, 270, 500, 44}, a.pass, Input::Text, true, 64);
  ui.textField("Repite la contraseña", {160, 350, 500, 44}, a.pass2, Input::Text, true, 64);

  const bool enviar = ui.button("Crear cuenta", {160, 420, 500, 48}, true) || ui.enterPressed();
  mostrarNota(ui, a, 486);

  if (ui.button("Volver", kBack)) {
    go(a, Screen::Inicio);
    return;
  }

  if (enviar) {
    if (a.pass != a.pass2) {
      fail(a, "Las contraseñas no coinciden");
      return;
    }
    switch (registrarUsuario(a.user, a.pass)) {
    case RegistroResult::Ok: {
      const std::string nombre = normalizarUsuario(a.user);
      go(a, Screen::Login, "Usuario registrado. Ya puedes iniciar sesión.", true);
      a.user = nombre;
      break;
    }
    case RegistroResult::CamposVacios:
      fail(a, "Completa el usuario y la contraseña");
      break;
    case RegistroResult::CaracteresInvalidos:
      fail(a, "No se permiten comillas ni barra invertida");
      break;
    case RegistroResult::YaExiste:
      fail(a, "Ese usuario ya existe");
      break;
    case RegistroResult::ErrorArchivo:
      fail(a, "No se pudo guardar en DATA/users.json");
      break;
    }
  }
}

void pantallaMenu(ui::Ui &ui, App &a) {
  encabezado(ui, "Hola, " + a.currentUser, "¿Qué quieres hacer?");

  if (ui.button("Calculadora", {230, 165, 360, 50}, true))
    go(a, Screen::Calculadora);
  if (ui.button("Generador de contraseñas", {230, 230, 360, 50}, true))
    go(a, Screen::Generador);
  if (ui.button("Validador de tarjetas", {230, 295, 360, 50}, true))
    go(a, Screen::Validador);
  if (ui.button("Cálculo de notas", {230, 360, 360, 50}, true))
    go(a, Screen::Notas);
  if (ui.button("Cerrar sesión", {230, 445, 360, 50})) {
    a.currentUser.clear();
    go(a, Screen::Inicio, "Sesión cerrada", true);
  }
}

void pantallaCalculadora(ui::Ui &ui, App &a) {
  encabezado(ui, "Calculadora", "Escribe dos números y elige la operación");

  ui.textField("Primer número", {130, 190, 270, 44}, a.calcA, Input::Number, false, 24);
  ui.textField("Segundo número", {420, 190, 270, 44}, a.calcB, Input::Number, false, 24);

  auto calcular_ = [&](Op op) {
    a.calcResult.clear();
    a.note.clear();
    double x = 0, y = 0;
    if (!parseNumero(a.calcA, x) || !parseNumero(a.calcB, y)) {
      fail(a, "Escribe dos números válidos");
      return;
    }
    CalcResult r = calcular(op, x, y);
    if (r.ok)
      a.calcResult = formatearNumero(r.value);
    else
      fail(a, r.error);
  };

  if (ui.button("Sumar", {106, 260, 140, 46}))
    calcular_(Op::Suma);
  if (ui.button("Restar", {262, 260, 140, 46}))
    calcular_(Op::Resta);
  if (ui.button("Multiplicar", {418, 260, 140, 46}))
    calcular_(Op::Multiplicacion);
  if (ui.button("Dividir", {574, 260, 140, 46}))
    calcular_(Op::Division);

  if (!a.calcResult.empty()) {
    ui.text("Resultado", 130, 340, Size::Small, Tone::Muted);
    ui.text(a.calcResult, 130, 364, Size::Title, Tone::Accent);
  } else {
    mostrarNota(ui, a, 372);
  }

  if (ui.button("Volver al menú", {90, 530, 190, 44}))
    go(a, Screen::Menu);
}

void pantallaGenerador(ui::Ui &ui, App &a) {
  encabezado(ui, "Generador de contraseñas", "Letras, números y símbolos al azar");

  ui.textField("Longitud (" + std::to_string(kPassMin) + " a " + std::to_string(kPassMax) + ")",
               {130, 190, 200, 44}, a.genLen, Input::Integer, false, 3);

  bool generar = ui.button("Generar", {350, 190, 160, 44}, true) || ui.enterPressed();
  if (!a.genPass.empty() && ui.button("Copiar", {526, 190, 160, 44})) {
    SDL_SetClipboardText(a.genPass.c_str());
    ok(a, "Contraseña copiada al portapapeles");
  }

  if (generar) {
    a.genPass.clear();
    a.note.clear();
    char *fin = nullptr;
    const long n = std::strtol(a.genLen.c_str(), &fin, 10);
    if (a.genLen.empty() || *fin != '\0' || n < kPassMin || n > kPassMax)
      fail(a, "Escribe una longitud entre " + std::to_string(kPassMin) + " y " + std::to_string(kPassMax));
    else
      a.genPass = generarPassword(static_cast<int>(n));
  }

  if (!a.genPass.empty()) {
    constexpr size_t kPorLinea = 30;
    const size_t filas = (a.genPass.size() + kPorLinea - 1) / kPorLinea;
    ui.panel({130, 270, 560, static_cast<float>(filas) * 30.0f + 24.0f});
    for (size_t i = 0; i < filas; ++i)
      ui.text(a.genPass.substr(i * kPorLinea, kPorLinea), 146, 282 + static_cast<float>(i) * 30.0f,
              Size::Body, Tone::Accent);
  }
  mostrarNota(ui, a, a.genPass.empty() ? 270.0f : 470.0f);

  if (ui.button("Volver al menú", {90, 530, 190, 44}))
    go(a, Screen::Menu);
}

void pantallaValidador(ui::Ui &ui, App &a) {
  encabezado(ui, "Validador de tarjetas", "Comprueba el número con el algoritmo de Luhn");

  ui.textField("Número de tarjeta", {130, 200, 560, 44}, a.card, Input::Card, false, 23);
  ui.text("Puedes pegar el número con Ctrl+V (Cmd+V en Mac)", 130, 252, Size::Small, Tone::Muted);

  const bool validar = ui.button("Validar", {130, 290, 560, 48}, true) || ui.enterPressed();
  if (validar) {
    a.note.clear();
    if (a.card.empty()) {
      a.cardState = 0;
      fail(a, "Escribe el número de la tarjeta");
    } else {
      a.cardState = luhnValido(a.card) ? 1 : 2;
    }
  }

  if (a.cardState == 1)
    ui.textCentered("El número pasa la verificación Luhn", kW / 2.0f, 366, Size::Body, Tone::Good);
  else if (a.cardState == 2)
    ui.textCentered("El número NO pasa la verificación Luhn", kW / 2.0f, 366, Size::Body, Tone::Bad);
  else
    mostrarNota(ui, a, 366);

  const auto aviso = ui.wrap("Solo se comprueba el dígito de control y la longitud (13 a 19 dígitos). "
                             "Eso no confirma que la tarjeta exista ni que tenga fondos.",
                             540, Size::Small);
  for (size_t i = 0; i < aviso.size(); ++i)
    ui.text(aviso[i], 140, 410 + static_cast<float>(i) * 22.0f, Size::Small, Tone::Muted);

  if (ui.button("Volver al menú", {90, 530, 190, 44}))
    go(a, Screen::Menu);
}

void pantallaNotas(ui::Ui &ui, App &a) {
  encabezado(ui, "Cálculo de notas", "Agrega las notas y calcula el promedio");

  ui.textField("Nota", {130, 190, 200, 44}, a.noteInput, Input::Number, false, 10);

  const bool agregar = ui.button("Agregar", {350, 190, 160, 44}, true) || ui.enterPressed();
  if (ui.button("Quitar última", {526, 190, 164, 44})) {
    if (!a.notes.empty())
      a.notes.pop_back();
    a.avg.clear();
    a.note.clear();
  }

  if (agregar) {
    double n = 0;
    a.avg.clear();
    a.note.clear();
    if (parseNumero(a.noteInput, n)) {
      a.notes.push_back(n);
      a.noteInput.clear();
    } else {
      fail(a, "Escribe una nota válida (por ejemplo 4.5)");
    }
  }

  if (ui.button("Calcular promedio", {130, 250, 270, 44}, true)) {
    a.note.clear();
    double p = 0;
    if (promedio(a.notes, p)) {
      char buf[48];
      std::snprintf(buf, sizeof buf, "%.2f", p);
      a.avg = buf;
    } else {
      a.avg.clear();
      fail(a, "Agrega al menos una nota");
    }
  }
  if (ui.button("Limpiar", {416, 250, 274, 44})) {
    a.notes.clear();
    a.avg.clear();
    a.note.clear();
  }

  ui.text("Notas ingresadas: " + std::to_string(a.notes.size()), 130, 316, Size::Small, Tone::Muted);
  std::string lista;
  for (size_t i = 0; i < a.notes.size(); ++i)
    lista += formatearNumero(a.notes[i]) + (i + 1 < a.notes.size() ? ", " : "");
  auto lineas = ui.wrap(lista, 560, Size::Body);
  constexpr size_t kMaxLineas = 3;
  const bool recortada = lineas.size() > kMaxLineas;
  if (recortada) {
    lineas.resize(kMaxLineas);
    lineas.back() += " ...";
  }
  for (size_t i = 0; i < lineas.size(); ++i)
    ui.text(lineas[i], 130, 342 + static_cast<float>(i) * 26.0f);

  if (!a.avg.empty()) {
    ui.text("Promedio: " + a.avg, 130, 440, Size::Title, Tone::Accent);
  } else {
    mostrarNota(ui, a, 450);
  }

  if (ui.button("Volver al menú", {90, 530, 190, 44}))
    go(a, Screen::Menu);
}

// ------------------------------------------------------------------ fuente

std::string buscarFuente() {
  namespace fs = std::filesystem;
  std::vector<std::string> candidatos;

  if (const char *base = SDL_GetBasePath()) {
    candidatos.push_back(std::string(base) + "assets/font.ttf");
    candidatos.push_back(std::string(base) + "../assets/font.ttf");
  }
  for (const char *c : {
           "assets/font.ttf",
           // Linux
           "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
           "/usr/share/fonts/TTF/DejaVuSans.ttf",
           "/usr/share/fonts/dejavu/DejaVuSans.ttf",
           "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
           "/usr/share/fonts/noto/NotoSans-Regular.ttf",
           // macOS
           "/System/Library/Fonts/Supplemental/Arial.ttf",
           "/Library/Fonts/Arial.ttf",
           "/System/Library/Fonts/Helvetica.ttc",
           // Windows
           "C:/Windows/Fonts/segoeui.ttf",
           "C:/Windows/Fonts/arial.ttf",
       })
    candidatos.emplace_back(c);

  for (const auto &c : candidatos) {
    std::error_code ec;
    if (fs::exists(c, ec))
      return c;
  }
  return "";
}

} // namespace

// ------------------------------------------------------------------ main

int main(int, char **) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::fprintf(stderr, "No se pudo iniciar SDL: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Overlite", kW, kH, 0);
  SDL_Renderer *renderer = window ? SDL_CreateRenderer(window, nullptr) : nullptr;
  if (!window || !renderer) {
    std::fprintf(stderr, "No se pudo crear la ventana: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }
  const bool vsync = SDL_SetRenderVSync(renderer, 1);

  if (!TTF_Init()) {
    std::fprintf(stderr, "No se pudo iniciar SDL3_ttf: %s\n", SDL_GetError());
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  ui::Ui ui;
  const std::string fuente = buscarFuente();
  if (fuente.empty() || !ui.init(window, renderer, fuente)) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Overlite",
                             "No se encontró una fuente .ttf.\n"
                             "Copia cualquier fuente TrueType a assets/font.ttf (junto al ejecutable o en la carpeta del proyecto).",
                             window);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_StartTextInput(window);

  App app;
  while (app.running) {
    ui.beginFrame();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT)
        app.running = false;
      ui.handleEvent(e);
    }

    ui.clearBackground();
    switch (app.screen) {
    case Screen::Inicio:
      pantallaInicio(ui, app);
      break;
    case Screen::Login:
      pantallaLogin(ui, app);
      break;
    case Screen::Registro:
      pantallaRegistro(ui, app);
      break;
    case Screen::Menu:
      pantallaMenu(ui, app);
      break;
    case Screen::Calculadora:
      pantallaCalculadora(ui, app);
      break;
    case Screen::Generador:
      pantallaGenerador(ui, app);
      break;
    case Screen::Validador:
      pantallaValidador(ui, app);
      break;
    case Screen::Notas:
      pantallaNotas(ui, app);
      break;
    }
    ui.endFrame();
    SDL_RenderPresent(renderer);

    if (app.pending != app.screen) {
      app.screen = app.pending;
      ui.resetFocus();
    }
    if (!vsync)
      SDL_Delay(16);
  }

  SDL_StopTextInput(window);
  ui.shutdown(); // antes de cerrar TTF y el renderizador
  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
