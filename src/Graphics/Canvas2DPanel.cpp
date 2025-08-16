#include "Canvas2DPanel.h"

Canvas2DPanel::Canvas2DPanel()
    : Panel("Viewport 2D") {
    // El rect final lo coloca el DockGroup en render(), aquí nada especial
}

void Canvas2DPanel::handleEvent(const SDL_Event& event) {
    // Pan sencillo con botón central (rueda)
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_MIDDLE) {
        panning = true;
        panStartX = event.button.x - offsetX;
        panStartY = event.button.y - offsetY;
    }
    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_MIDDLE) {
        panning = false;
    }
    else if (event.type == SDL_MOUSEMOTION && panning) {
        offsetX = event.motion.x - panStartX;
        offsetY = event.motion.y - panStartY;
    }
}

void Canvas2DPanel::drawGrid(SDL_Renderer* renderer, const SDL_Rect& area) {
    // Fondo del canvas
    SDL_SetRenderDrawColor(renderer, 28, 28, 30, 255);
    SDL_RenderFillRect(renderer, &area);

    // Rejilla
    const int grid = 32;
    // Líneas finas
    SDL_SetRenderDrawColor(renderer, 50, 50, 55, 255);
    for (int x = area.x + (offsetX % grid); x < area.x + area.w; x += grid) {
        SDL_RenderDrawLine(renderer, x, area.y, x, area.y + area.h);
    }
    for (int y = area.y + (offsetY % grid); y < area.y + area.h; y += grid) {
        SDL_RenderDrawLine(renderer, area.x, y, area.x + area.w, y);
    }

    // Ejes más marcados (cada 10 celdas)
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    int big = grid * 10;
    for (int x = area.x + (offsetX % big); x < area.x + area.w; x += big) {
        SDL_RenderDrawLine(renderer, x, area.y, x, area.y + area.h);
    }
    for (int y = area.y + (offsetY % big); y < area.y + area.h; y += big) {
        SDL_RenderDrawLine(renderer, area.x, y, area.x + area.w, y);
    }
}

void Canvas2DPanel::render(SDL_Renderer* renderer, TTF_Font* /*font*/) {
    // El DockGroup ya nos configuró rect = { x, y+25, w, h-25 }
    SDL_Rect canvasArea = rect;
    drawGrid(renderer, canvasArea);
}

