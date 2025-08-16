#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Panel.h"

struct DockGroup {
    SDL_Rect rect;
    std::vector<std::shared_ptr<Panel>> tabs;
    int activeTabIndex = 0;
    bool dragging = false;
    int dragOffsetX = 0;
    int dragOffsetY = 0;

    DockGroup() {
        rect.x = 0;
        rect.y = 0;
        rect.w = 0;
        rect.h = 0;
    }

    void handleEvent(const SDL_Event& event);
    void render(SDL_Renderer* renderer, TTF_Font* font);
};

