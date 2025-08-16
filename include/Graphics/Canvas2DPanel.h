#pragma once
#include "Panel.h"

class Canvas2DPanel : public Panel {
public:
    Canvas2DPanel();

    void handleEvent(const SDL_Event& event) override;
    void render(SDL_Renderer* renderer, TTF_Font* font) override;

private:
    bool panning = false;
    int panStartX = 0;
    int panStartY = 0;
    int offsetX = 0;
    int offsetY = 0;

    void drawGrid(SDL_Renderer* renderer, const SDL_Rect& area);
};

