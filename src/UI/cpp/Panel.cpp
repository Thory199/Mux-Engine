#include "Panel.h"
#include <algorithm> // std::max

Panel::Panel() = default;

Panel::Panel(const SDL_Rect& r, const std::string& t)
    : rect(r), title(t) {
}

Panel::Panel(const SDL_Rect& r, const char* t)
    : rect(r), title(t ? t : "") {
}

Panel::Panel(int x, int y, int w, int h, const std::string& t)
    : rect{ x, y, w, h }, title(t) {
}

Panel::Panel(int x, int y, int w, int h, const char* t)
    : rect{ x, y, w, h }, title(t ? t : "") {
}

Panel::Panel(std::initializer_list<int> dims) {
    // Esperamos 4 números: x, y, w, h
    int tmp[4] = { 0, 0, 100, 80 };
    int i = 0;
    for (int v : dims) { if (i < 4) tmp[i++] = v; }
    rect = SDL_Rect{ tmp[0], tmp[1], tmp[2], tmp[3] };
}

Panel::Panel(std::initializer_list<int> dims, const char* t)
    : Panel(dims) {
    if (t) title = t;
}

bool Panel::mouseInRect(int x, int y, const SDL_Rect& r) const {
    return (x >= r.x) && (y >= r.y) && (x < r.x + r.w) && (y < r.y + r.h);
}

void Panel::clampMinSize() {
    if (rect.w < minW) rect.w = minW;
    if (rect.h < minH) rect.h = minH;
}

void Panel::setMinSize(int w, int h) {
    minW = std::max(1, w);
    minH = std::max(1, h);
    clampMinSize();
}

void Panel::handleEvent(const SDL_Event& e) {
    if (!visible) return;

    int mx = 0, my = 0;
    SDL_GetMouseState(&mx, &my);

    SDL_Rect titleBar{ rect.x, rect.y, rect.w, 24 };
    SDL_Rect rightGrip{ rect.x + rect.w - kResizeGrip, rect.y, kResizeGrip, rect.h };
    SDL_Rect bottomGrip{ rect.x, rect.y + rect.h - kResizeGrip, rect.w, kResizeGrip };
    SDL_Rect cornerGrip{
        rect.x + rect.w - kResizeGrip,
        rect.y + rect.h - kResizeGrip,
        kResizeGrip, kResizeGrip
    };

    switch (e.type) {
    case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == SDL_BUTTON_LEFT) {
            if (mouseInRect(mx, my, cornerGrip) || mouseInRect(mx, my, rightGrip) || mouseInRect(mx, my, bottomGrip)) {
                resizing = true;
                resizeOffsetX = (rect.x + rect.w) - mx;
                resizeOffsetY = (rect.y + rect.h) - my;
            }
            else if (mouseInRect(mx, my, titleBar)) {
                dragging = true;
                dragOffsetX = mx - rect.x;
                dragOffsetY = my - rect.y;
            }
        }
        break;

    case SDL_MOUSEBUTTONUP:
        if (e.button.button == SDL_BUTTON_LEFT) {
            resizing = false;
            dragging = false;
        }
        break;

    case SDL_MOUSEMOTION:
        if (resizing) {
            int newW = (mx + resizeOffsetX) - rect.x;
            int newH = (my + resizeOffsetY) - rect.y;
            rect.w = std::max(newW, minW);
            rect.h = std::max(newH, minH);
        }
        else if (dragging) {
            rect.x = mx - dragOffsetX;
            rect.y = my - dragOffsetY;
        }
        break;

    default: break;
    }
}

void Panel::render(SDL_Renderer* renderer, TTF_Font* font) {
    if (!visible) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Fondo
    SDL_SetRenderDrawColor(renderer, 30, 30, 35, 230);
    SDL_RenderFillRect(renderer, &rect);

    // Borde
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);

    // Barra de título
    SDL_Rect titleBar{ rect.x, rect.y, rect.w, 24 };
    SDL_SetRenderDrawColor(renderer, 50, 50, 58, 255);
    SDL_RenderFillRect(renderer, &titleBar);

    // Título
    if (font && !title.empty()) {
        SDL_Color col{ 235, 235, 235, 255 };
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, title.c_str(), col);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            if (tex) {
                SDL_Rect dst{
                    titleBar.x + 8,
                    titleBar.y + (titleBar.h - surf->h) / 2,
                    surf->w, surf->h
                };
                SDL_RenderCopy(renderer, tex, nullptr, &dst);
                SDL_DestroyTexture(tex);
            }
            SDL_FreeSurface(surf);
        }
    }

    // Guías de resize (opcionales)
    SDL_SetRenderDrawColor(renderer, 90, 90, 100, 255);
    SDL_Rect rightGrip{ rect.x + rect.w - kResizeGrip, rect.y, kResizeGrip, rect.h };
    SDL_Rect bottomGrip{ rect.x, rect.y + rect.h - kResizeGrip, rect.w, kResizeGrip };
    SDL_RenderFillRect(renderer, &rightGrip);
    SDL_RenderFillRect(renderer, &bottomGrip);

    SDL_SetRenderDrawColor(renderer, 120, 120, 130, 255);
    SDL_Rect cornerGrip{
        rect.x + rect.w - kResizeGrip,
        rect.y + rect.h - kResizeGrip,
        kResizeGrip, kResizeGrip
    };
    SDL_RenderFillRect(renderer, &cornerGrip);
}
