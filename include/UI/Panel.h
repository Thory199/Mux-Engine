#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

struct Panel {
    std::string title;
    SDL_Rect rect{ 0,0,0,0 };

    // Para paneles dentro de docks, no usamos drag aquí (lo maneja DockGroup).
    bool dragging = false;
    int dragOffsetX = 0;
    int dragOffsetY = 0;

    Panel(const std::string& t = "Panel") : title(t) {}
    virtual ~Panel() = default;

    // 🔹 HAZLOS VIRTUALES para poder override en Canvas2DPanel
    virtual void handleEvent(const SDL_Event& event) {}
    virtual void render(SDL_Renderer* renderer, TTF_Font* font) {}
};
