#include "DockGroup.h"
#include "DockManager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm> // Para std::max

extern SDL_Window* gWindow;

const int MENU_BAR_HEIGHT = 60;
const int SNAP_DISTANCE = 10;

void DockGroup::handleEvent(const SDL_Event& event) {
    if (tabs.empty()) return;

    int mx = event.type == SDL_MOUSEMOTION ? event.motion.x : event.button.x;
    int my = event.type == SDL_MOUSEMOTION ? event.motion.y : event.button.y;

    SDL_Rect titleBar = { rect.x, rect.y, rect.w, 25 };

    // Detección de clic en esquina inferior derecha para redimensionar
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Rect resizeZone = { rect.x + rect.w - 10, rect.y + rect.h - 10, 10, 10 };
        if (mx >= resizeZone.x && mx <= resizeZone.x + resizeZone.w &&
            my >= resizeZone.y && my <= resizeZone.y + resizeZone.h) {
            resizing = true;
            resizeOffsetX = (rect.x + rect.w) - mx;
            resizeOffsetY = (rect.y + rect.h) - my;
            return;
        }

        if (mx >= titleBar.x && mx <= titleBar.x + titleBar.w &&
            my >= titleBar.y && my <= titleBar.y + titleBar.h) {

            int tabX = rect.x + 5;
            bool clickedTab = false;

            for (size_t i = 0; i < tabs.size(); ++i) {
                int textW = static_cast<int>(tabs[i]->title.size()) * 8;
                int tabW = textW + 30;

                SDL_Rect tabRect = { tabX, rect.y, tabW, 25 };
                SDL_Rect closeBtn = { tabX + tabW - 20, rect.y + 5, 10, 10 };

                if (mx >= closeBtn.x && mx <= closeBtn.x + closeBtn.w &&
                    my >= closeBtn.y && my <= closeBtn.y + closeBtn.h) {
                    tabs.erase(tabs.begin() + i);
                    if (tabs.empty()) {
                        rect = { 0,0,0,0 };
                    }
                    else if (activeTabIndex >= static_cast<int>(tabs.size())) {
                        activeTabIndex = 0;
                    }
                    return;
                }

                if (mx >= tabRect.x && mx <= tabRect.x + tabRect.w) {
                    activeTabIndex = static_cast<int>(i);
                    clickedTab = true;
                    return;
                }
                tabX += tabW + 5;
            }

            if (!clickedTab) {
                dragging = true;
                dragOffsetX = mx - rect.x;
                dragOffsetY = my - rect.y;
            }
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        dragging = false;
        resizing = false;
    }
    else if (event.type == SDL_MOUSEMOTION) {
        if (dragging) {
            rect.x = mx - dragOffsetX;
            rect.y = my - dragOffsetY;

            int winW, winH;
            SDL_GetWindowSize(gWindow, &winW, &winH);

            if (rect.x < 0) rect.x = 0;
            if (rect.y < MENU_BAR_HEIGHT) rect.y = MENU_BAR_HEIGHT;
            if (rect.x + rect.w > winW) rect.x = winW - rect.w;
            if (rect.y + rect.h > winH) rect.y = winH - rect.h;

            for (auto& other : DockManager::getDocks()) {
                if (&other == this || other.rect.w == 0 || other.rect.h == 0) continue;

                if (abs((rect.x + rect.w) - other.rect.x) <= SNAP_DISTANCE)
                    rect.x = other.rect.x - rect.w;
                else if (abs(rect.x - (other.rect.x + other.rect.w)) <= SNAP_DISTANCE)
                    rect.x = other.rect.x + other.rect.w;

                if (abs((rect.y + rect.h) - other.rect.y) <= SNAP_DISTANCE)
                    rect.y = other.rect.y - rect.h;
                else if (abs(rect.y - (other.rect.y + other.rect.h)) <= SNAP_DISTANCE)
                    rect.y = other.rect.y + other.rect.h;
            }
        }
        else if (resizing) {
            rect.w = mx - rect.x + resizeOffsetX;
            rect.h = my - rect.y + resizeOffsetY;

            if (rect.w < 100) rect.w = 100;
            if (rect.h < 100) rect.h = 100;
        }
    }

    if (activeTabIndex < static_cast<int>(tabs.size())) {
        tabs[activeTabIndex]->rect = { rect.x, rect.y + 25, rect.w, rect.h - 25 };
        tabs[activeTabIndex]->handleEvent(event);
    }
}

void DockGroup::render(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color gray = { 180, 180, 180, 255 };

    int totalTabWidth = 10;
    for (size_t i = 0; i < tabs.size(); ++i) {
        int textW = static_cast<int>(tabs[i]->title.size()) * 8;
        int tabW = textW + 30;
        totalTabWidth += tabW + 5;
    }
    if (totalTabWidth > rect.w) rect.w = totalTabWidth;

    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Borde negro de 2px
    for (int i = 0; i < 2; ++i) {
        SDL_Rect border = { rect.x - i, rect.y - i, rect.w + i * 2, rect.h + i * 2 };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &border);
    }

    SDL_Rect tabBar = { rect.x, rect.y, rect.w, 25 };
    SDL_SetRenderDrawColor(renderer, 70, 70, 80, 255);
    SDL_RenderFillRect(renderer, &tabBar);

    int tabX = rect.x + 5;
    for (size_t i = 0; i < tabs.size(); ++i) {
        int textW = static_cast<int>(tabs[i]->title.size()) * 8;
        int tabW = textW + 30;
        SDL_Rect tabRect = { tabX, rect.y, tabW, 25 };
        SDL_SetRenderDrawColor(renderer, 90, 90, 100, 255);
        SDL_RenderFillRect(renderer, &tabRect);

        SDL_Surface* surface = TTF_RenderText_Solid(font, tabs[i]->title.c_str(), white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = { tabX + 5, rect.y + 4, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        SDL_Surface* xSurf = TTF_RenderText_Solid(font, "X", gray);
        SDL_Texture* xTex = SDL_CreateTextureFromSurface(renderer, xSurf);
        SDL_Rect xRect = { tabX + tabW - 15, rect.y + 4, xSurf->w, xSurf->h };
        SDL_RenderCopy(renderer, xTex, nullptr, &xRect);
        SDL_FreeSurface(xSurf);
        SDL_DestroyTexture(xTex);

        tabX += tabW + 5;
    }

    if (activeTabIndex < static_cast<int>(tabs.size())) {
        tabs[activeTabIndex]->rect = { rect.x, rect.y + 25, rect.w, rect.h - 25 };
        tabs[activeTabIndex]->render(renderer, font);
    }

    // Dibujar indicador de redimensionar en esquina
    SDL_Rect resizeCorner = { rect.x + rect.w - 10, rect.y + rect.h - 10, 10, 10 };
    SDL_SetRenderDrawColor(renderer, 120, 120, 130, 255);
    SDL_RenderFillRect(renderer, &resizeCorner);
}
