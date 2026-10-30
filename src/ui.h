// Mini toolkit de interfaz sobre SDL3 + SDL3_ttf (modo inmediato).
// Cada fotograma: beginFrame() -> handleEvent() por cada evento -> dibujar widgets -> endFrame().
#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace ui {

enum class Size { Small, Body, Title };
enum class Tone { Normal, Muted, Good, Bad, Accent };
enum class Input { Text, Number, Integer, Card };

class Ui {
public:
  ~Ui() { shutdown(); }

  bool init(SDL_Window *window, SDL_Renderer *renderer, const std::string &fontPath);
  void shutdown();

  void beginFrame();
  void handleEvent(const SDL_Event &e);
  void endFrame();

  // Dibujo
  void clearBackground();
  void panel(const SDL_FRect &r);
  void accentLine(float cx, float y, float w); // barra roja decorativa
  void text(const std::string &s, float x, float y, Size size = Size::Body, Tone tone = Tone::Normal);
  void textCentered(const std::string &s, float cx, float y, Size size = Size::Body, Tone tone = Tone::Normal);
  float textWidth(const std::string &s, Size size);
  std::vector<std::string> wrap(const std::string &s, float maxWidth, Size size = Size::Body);

  // Widgets
  bool button(const std::string &label, const SDL_FRect &r, bool primary = false);
  void textField(const std::string &label, const SDL_FRect &r, std::string &value,
                 Input kind = Input::Text, bool mask = false, size_t maxLen = 64);

  // Estado de entrada
  bool enterPressed() const { return enter_; }
  void resetFocus() { focus_ = 0; }

private:
  struct Cached {
    SDL_Texture *tex = nullptr;
    float w = 0, h = 0;
  };

  TTF_Font *font(Size s) const;
  Cached *getText(const std::string &s, Size size, SDL_Color c);
  void clearCache();
  bool inside(const SDL_FRect &r, float x, float y) const;
  void fill(const SDL_FRect &r, SDL_Color c);
  void outline(const SDL_FRect &r, SDL_Color c);

  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;
  TTF_Font *fontSmall_ = nullptr, *fontBody_ = nullptr, *fontTitle_ = nullptr;
  std::unordered_map<std::string, Cached> cache_;

  // Entrada del fotograma actual
  float mouseX_ = 0, mouseY_ = 0;
  bool clicked_ = false;
  float clickX_ = 0, clickY_ = 0;
  std::string typed_;
  int backspaces_ = 0;
  bool paste_ = false, enter_ = false, tab_ = false;

  // Foco entre campos de texto
  int focus_ = 0;
  int fieldCounter_ = 0;
  int fieldsLastFrame_ = 0;
};

} // namespace ui
