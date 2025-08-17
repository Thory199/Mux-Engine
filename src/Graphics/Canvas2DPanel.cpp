#include "Canvas2DPanel.h"
#include <SDL.h>
#include <SDL_ttf.h>

Canvas2DPanel::Canvas2DPanel()
    : Panel(100, 100, 320, 240, "Canvas 2D")
{
}

Canvas2DPanel::Canvas2DPanel(const SDL_Rect& r, const char* t)
    : Panel(r, t)
{
}

Canvas2DPanel::Canvas2DPanel(int x, int y, int w, int h, const char* t)
    : Panel(x, y, w, h, t)
{
}

void Canvas2DPanel::handleEvent(const SDL_Event& e) {
    // Reusa la lógica base (drag/resize). Añade tus extras si quieres.
    Panel::handleEvent(e);

    // Aquí puedes meter lógica específica del canvas (p. ej. pintar con el mouse)
    // ...
}

void Canvas2DPanel::render(SDL_Renderer* renderer, TTF_Font* font) {
    // Dibujo base del panel
    Panel::render(renderer, font);

    // Aquí dibuja el contenido del canvas (grid, etc.)
    // ...
}
