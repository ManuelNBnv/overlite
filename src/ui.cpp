#include "ui.h"

#include <cstdio>

namespace ui {
namespace {

const SDL_Color kBg{20, 12, 14, 255};
const SDL_Color kPanel{32, 20, 23, 255};
const SDL_Color kBorder{78, 44, 50, 255};
const SDL_Color kField{24, 15, 17, 255};
const SDL_Color kText{244, 236, 236, 255};
const SDL_Color kMuted{176, 146, 150, 255};
const SDL_Color kAccent{226, 54, 62, 255};          // rojo principal (botones, bordes)
const SDL_Color kAccentHover{245, 84, 90, 255};
const SDL_Color kAccentText{255, 107, 112, 255};    // rojo más claro para texto
const SDL_Color kButton{56, 32, 37, 255};
const SDL_Color kButtonHover{82, 44, 52, 255};
const SDL_Color kGood{110, 205, 150, 255};
const SDL_Color kBad{255, 176, 96, 255};            // ámbar: se distingue del rojo del tema

const float kPad = 12.0f;

SDL_Color toneColor(Tone t) {
  switch (t) {
  case Tone::Muted:
    return kMuted;
  case Tone::Good:
    return kGood;
  case Tone::Bad:
    return kBad;
  case Tone::Accent:
    return kAccentText;
  default:
    return kText;
  }
}

bool esContinuacion(unsigned char c) { return (c & 0xC0) == 0x80; }

// Borra el último carácter UTF-8 (no solo el último byte).
void quitarUltimo(std::string &s) {
  while (!s.empty() && esContinuacion(static_cast<unsigned char>(s.back())))
    s.pop_back();
  if (!s.empty())
    s.pop_back();
}

void quitarPrimero(std::string &s) {
  size_t i = 1;
  while (i < s.size() && esContinuacion(static_cast<unsigned char>(s[i])))
    ++i;
  s.erase(0, i);
}

size_t contarCaracteres(const std::string &s) {
  size_t n = 0;
  for (unsigned char c : s)
    if (!esContinuacion(c))
      ++n;
  return n;
}

bool permitido(unsigned char c, Input kind) {
  switch (kind) {
  case Input::Integer:
    return c >= '0' && c <= '9';
  case Input::Number:
    return (c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.' || c == ',';
  case Input::Card:
    return (c >= '0' && c <= '9') || c == ' ' || c == '-';
  default:
    return c >= 0x20 && c != 0x7f; // los bytes >= 0x80 (UTF-8) pasan
  }
}

void agregarFiltrado(std::string &dest, const std::string &src, Input kind, size_t maxLen) {
  for (unsigned char c : src) {
    if (dest.size() >= maxLen)
      break;
    if (permitido(c, kind))
      dest += static_cast<char>(c);
  }
}

} // namespace

// ------------------------------------------------------------------ ciclo de vida

bool Ui::init(SDL_Window *window, SDL_Renderer *renderer, const std::string &fontPath) {
  window_ = window;
  renderer_ = renderer;
  fontSmall_ = TTF_OpenFont(fontPath.c_str(), 16.0f);
  fontBody_ = TTF_OpenFont(fontPath.c_str(), 20.0f);
  fontTitle_ = TTF_OpenFont(fontPath.c_str(), 36.0f);
  if (!fontSmall_ || !fontBody_ || !fontTitle_) {
    shutdown();
    return false;
  }
  return true;
}

void Ui::shutdown() {
  clearCache();
  if (fontSmall_) TTF_CloseFont(fontSmall_);
  if (fontBody_) TTF_CloseFont(fontBody_);
  if (fontTitle_) TTF_CloseFont(fontTitle_);
  fontSmall_ = fontBody_ = fontTitle_ = nullptr;
}

void Ui::beginFrame() {
  SDL_GetMouseState(&mouseX_, &mouseY_);
  clicked_ = false;
  typed_.clear();
  backspaces_ = 0;
  paste_ = enter_ = tab_ = false;
  fieldCounter_ = 0;
}

void Ui::handleEvent(const SDL_Event &e) {
  switch (e.type) {
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    if (e.button.button == SDL_BUTTON_LEFT) {
      clicked_ = true;
      clickX_ = e.button.x;
      clickY_ = e.button.y;
    }
    break;
  case SDL_EVENT_TEXT_INPUT:
    typed_ += e.text.text;
    break;
  case SDL_EVENT_KEY_DOWN:
    if (e.key.key == SDLK_BACKSPACE) {
      ++backspaces_;
    } else if (e.key.key == SDLK_RETURN || e.key.key == SDLK_KP_ENTER) {
      if (!e.key.repeat)
        enter_ = true;
    } else if (e.key.key == SDLK_TAB) {
      if (!e.key.repeat)
        tab_ = true;
    } else if (e.key.key == SDLK_V && (e.key.mod & (SDL_KMOD_CTRL | SDL_KMOD_GUI))) {
      paste_ = true;
    }
    break;
  default:
    break;
  }
}

void Ui::endFrame() {
  fieldsLastFrame_ = fieldCounter_;
  if (tab_ && fieldsLastFrame_ > 0)
    focus_ = (focus_ + 1) % fieldsLastFrame_;
}

// ------------------------------------------------------------------ utilidades

TTF_Font *Ui::font(Size s) const {
  switch (s) {
  case Size::Small:
    return fontSmall_;
  case Size::Title:
    return fontTitle_;
  default:
    return fontBody_;
  }
}

bool Ui::inside(const SDL_FRect &r, float x, float y) const {
  return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

void Ui::fill(const SDL_FRect &r, SDL_Color c) {
  SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(renderer_, &r);
}

void Ui::outline(const SDL_FRect &r, SDL_Color c) {
  SDL_SetRenderDrawColor(renderer_, c.r, c.g, c.b, c.a);
  SDL_RenderRect(renderer_, &r);
}

void Ui::clearCache() {
  for (auto &kv : cache_)
    if (kv.second.tex)
      SDL_DestroyTexture(kv.second.tex);
  cache_.clear();
}

// Las texturas de texto se guardan: renderizar texto con TTF cada fotograma es caro.
Ui::Cached *Ui::getText(const std::string &s, Size size, SDL_Color c) {
  if (s.empty())
    return nullptr;

  char prefix[32];
  std::snprintf(prefix, sizeof prefix, "%d:%02x%02x%02x:", static_cast<int>(size), c.r, c.g, c.b);
  const std::string key = std::string(prefix) + s;

  auto it = cache_.find(key);
  if (it != cache_.end())
    return &it->second;

  if (cache_.size() > 300)
    clearCache();

  SDL_Surface *surf = TTF_RenderText_Blended(font(size), s.c_str(), 0, c);
  if (!surf)
    return nullptr;
  SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer_, surf);
  SDL_DestroySurface(surf);
  if (!tex)
    return nullptr;

  Cached cached;
  cached.tex = tex;
  SDL_GetTextureSize(tex, &cached.w, &cached.h);
  return &cache_.emplace(key, cached).first->second;
}

float Ui::textWidth(const std::string &s, Size size) {
  if (s.empty())
    return 0;
  int w = 0, h = 0;
  TTF_GetStringSize(font(size), s.c_str(), 0, &w, &h);
  return static_cast<float>(w);
}

std::vector<std::string> Ui::wrap(const std::string &s, float maxWidth, Size size) {
  std::vector<std::string> lines;
  std::string line, word;

  auto flushWord = [&]() {
    if (word.empty())
      return;
    std::string candidate = line.empty() ? word : line + " " + word;
    if (!line.empty() && textWidth(candidate, size) > maxWidth) {
      lines.push_back(line);
      line = word;
    } else {
      line = candidate;
    }
    word.clear();
  };

  for (char c : s) {
    if (c == ' ')
      flushWord();
    else
      word += c;
  }
  flushWord();
  if (!line.empty())
    lines.push_back(line);
  return lines;
}

// ------------------------------------------------------------------ dibujo

void Ui::clearBackground() {
  SDL_SetRenderDrawColor(renderer_, kBg.r, kBg.g, kBg.b, kBg.a);
  SDL_RenderClear(renderer_);
}

void Ui::accentLine(float cx, float y, float w) {
  SDL_FRect r{cx - w / 2.0f, y, w, 3.0f};
  fill(r, kAccent);
}

void Ui::panel(const SDL_FRect &r) {
  fill(r, kPanel);
  outline(r, kBorder);
}

void Ui::text(const std::string &s, float x, float y, Size size, Tone tone) {
  Cached *c = getText(s, size, toneColor(tone));
  if (!c)
    return;
  SDL_FRect dst{x, y, c->w, c->h};
  SDL_RenderTexture(renderer_, c->tex, nullptr, &dst);
}

void Ui::textCentered(const std::string &s, float cx, float y, Size size, Tone tone) {
  Cached *c = getText(s, size, toneColor(tone));
  if (!c)
    return;
  SDL_FRect dst{cx - c->w / 2.0f, y, c->w, c->h};
  SDL_RenderTexture(renderer_, c->tex, nullptr, &dst);
}

// ------------------------------------------------------------------ widgets

bool Ui::button(const std::string &label, const SDL_FRect &r, bool primary) {
  const bool hover = inside(r, mouseX_, mouseY_);
  SDL_Color bg = primary ? (hover ? kAccentHover : kAccent) : (hover ? kButtonHover : kButton);
  fill(r, bg);
  outline(r, primary ? bg : (hover ? kAccent : kBorder));

  SDL_Color fg = primary ? SDL_Color{255, 246, 246, 255} : kText;
  if (Cached *c = getText(label, Size::Body, fg)) {
    SDL_FRect dst{r.x + (r.w - c->w) / 2.0f, r.y + (r.h - c->h) / 2.0f, c->w, c->h};
    SDL_RenderTexture(renderer_, c->tex, nullptr, &dst);
  }
  return clicked_ && inside(r, clickX_, clickY_);
}

void Ui::textField(const std::string &label, const SDL_FRect &r, std::string &value,
                   Input kind, bool mask, size_t maxLen) {
  const int id = fieldCounter_++;

  if (clicked_ && inside(r, clickX_, clickY_))
    focus_ = id;
  const bool focused = (focus_ == id);

  if (focused) {
    for (int i = 0; i < backspaces_; ++i)
      quitarUltimo(value);
    if (!typed_.empty())
      agregarFiltrado(value, typed_, kind, maxLen);
    if (paste_) {
      if (char *clip = SDL_GetClipboardText()) {
        std::string pegado(clip);
        SDL_free(clip);
        const size_t nl = pegado.find_first_of("\r\n");
        if (nl != std::string::npos)
          pegado.erase(nl);
        agregarFiltrado(value, pegado, kind, maxLen);
      }
    }
  }

  // Etiqueta encima del campo
  text(label, r.x, r.y - 26.0f, Size::Small, Tone::Muted);

  fill(r, kField);
  outline(r, focused ? kAccent : kBorder);

  std::string shown = mask ? std::string(contarCaracteres(value), '*') : value;
  const float maxW = r.w - 2.0f * kPad;
  while (!shown.empty() && textWidth(shown, Size::Body) > maxW)
    quitarPrimero(shown); // si no cabe, se ve el final (donde se está escribiendo)

  float textW = 0;
  if (Cached *c = getText(shown, Size::Body, kText)) {
    SDL_FRect dst{r.x + kPad, r.y + (r.h - c->h) / 2.0f, c->w, c->h};
    SDL_RenderTexture(renderer_, c->tex, nullptr, &dst);
    textW = c->w;
  }

  if (focused && ((SDL_GetTicks() / 500) % 2 == 0)) {
    const float cx = r.x + kPad + textW + 2.0f;
    SDL_SetRenderDrawColor(renderer_, kText.r, kText.g, kText.b, 255);
    SDL_RenderLine(renderer_, cx, r.y + 9.0f, cx, r.y + r.h - 9.0f);
  }
}

} // namespace ui
