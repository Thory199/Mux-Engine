#include "Panel.h"


void Panel::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mx = event.button.x;
        int my = event.button.y;
        SDL_Rect titleBar = { rect.x, rect.y, rect.w, 25 };
        if (mx >= titleBar.x && mx <= titleBar.x + titleBar.w &&
            my >= titleBar.y && my <= titleBar.y + titleBar.h) {
            dragging = true;
            dragOffsetX = mx - rect.x;
            dragOffsetY = my - rect.y;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        dragging = false;
    }
    else if (event.type == SDL_MOUSEMOTION && dragging) {
        int mx = event.motion.x;
        int my = event.motion.y;
        rect.x = mx - dragOffsetX;
        rect.y = my - dragOffsetY;
    }
}

void Panel::render(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color white = { 255, 255, 255, 255 };

    // Fondo panel
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Barra de título
    SDL_Rect titleBar = { rect.x, rect.y, rect.w, 25 };
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderFillRect(renderer, &titleBar);

    // Texto del título
    SDL_Surface* surface = TTF_RenderText_Solid(font, title.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = { rect.x + 5, rect.y + 4, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
