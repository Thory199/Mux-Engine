#pragma once
#include "Panel.h"

class Canvas2DPanel : public Panel {
public:
    // Constructor ejemplo (ajusta rect/título si quieres)
    Canvas2DPanel();
    explicit Canvas2DPanel(const SDL_Rect& r, const char* t = "Canvas 2D");
    Canvas2DPanel(int x, int y, int w, int h, const char* t = "Canvas 2D");

    // Asegura que override coincida EXACTO
    void handleEvent(const SDL_Event& e) override;
    void render(SDL_Renderer* renderer, TTF_Font* font) override;
};

