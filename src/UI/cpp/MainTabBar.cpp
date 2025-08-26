#include <algorithm>
#include "MainTabBar.h"
#include <SDL2/SDL_image.h>
#include <windows.h>

inline int MaxInt(int a, int b) {
    return (a > b) ? a : b;
}

std::vector<MainTab> MainTabBar::tabs;
int MainTabBar::activeTabIndex = 0;

SDL_Texture* MainTabBar::texClose = nullptr;
SDL_Texture* MainTabBar::texCloseActive = nullptr;
SDL_Texture* MainTabBar::texMinimize = nullptr;
SDL_Texture* MainTabBar::texMinimizeActive = nullptr;
SDL_Texture* MainTabBar::texMaximize = nullptr;
SDL_Texture* MainTabBar::texMaximizeActive = nullptr;

bool MainTabBar::hoverClose = false;
bool MainTabBar::hoverMinimize = false;
bool MainTabBar::hoverMaximize = false;

int mouseX = 0;
int mouseY = 0;

void MainTabBar::initialize(SDL_Renderer* renderer) {
    tabs.clear();
    tabs.push_back({ "ME1 Project", true });
    activeTabIndex = 0;

    auto loadTexture = [&](const char* path) -> SDL_Texture* {
        SDL_Surface* surface = IMG_Load(path);
        if (!surface) return nullptr;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return tex;
        };

    texClose = loadTexture("Data/Interface/close_idle.png");
    texCloseActive = loadTexture("Data/Interface/close_active.png");
    texMinimize = loadTexture("Data/Interface/minimize_idle.png");
    texMinimizeActive = loadTexture("Data/Interface/minimize_active.png");
    texMaximize = loadTexture("Data/Interface/maximize_idle.png");
    texMaximizeActive = loadTexture("Data/Interface/maximize_active.png");
}

SDL_Rect MainTabBar::getTabRect(int index) {
    int x = 40;
    for (int i = 0; i < index; ++i) {
        x += static_cast<int>(tabs[i].title.size()) * 8 + 40;
    }
    int width = static_cast<int>(tabs[index].title.size()) * 8 + 30;
    return { x, 0, width, 25 };
}

void MainTabBar::render(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color gray = { 180, 180, 180, 255 };

    for (size_t i = 0; i < tabs.size(); ++i) {
        SDL_Rect tabRect = getTabRect(static_cast<int>(i));
        SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        SDL_RenderFillRect(renderer, &tabRect);

        SDL_Surface* surface = TTF_RenderText_Solid(font, tabs[i].title.c_str(), white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = { tabRect.x + 5, tabRect.y + 5, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        if (i != 0) {
            SDL_Surface* xSurf = TTF_RenderText_Solid(font, "X", gray);
            SDL_Texture* xTex = SDL_CreateTextureFromSurface(renderer, xSurf);
            SDL_Rect xRect = { tabRect.x + tabRect.w - 15, tabRect.y + 5, xSurf->w, xSurf->h };
            SDL_RenderCopy(renderer, xTex, nullptr, &xRect);
            SDL_FreeSurface(xSurf);
            SDL_DestroyTexture(xTex);
        }

        if (static_cast<int>(i) == activeTabIndex) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
            SDL_RenderDrawRect(renderer, &tabRect);
        }
    }

    // Botones ventana
    int winW = 0, winH = 0;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    SDL_Rect rectMin = { winW - 90, 0, 30, 25 };
    SDL_Rect rectMax = { winW - 60, 0, 30, 25 };
    SDL_Rect rectClose = { winW - 30, 0, 30, 25 };

    SDL_Point mousePoint = { mouseX, mouseY };
    hoverMinimize = SDL_PointInRect(&mousePoint, &rectMin);
    hoverMaximize = SDL_PointInRect(&mousePoint, &rectMax);
    hoverClose = SDL_PointInRect(&mousePoint, &rectClose);

    SDL_RenderCopy(renderer, hoverMinimize ? texMinimizeActive : texMinimize, nullptr, &rectMin);
    SDL_RenderCopy(renderer, hoverMaximize ? texMaximizeActive : texMaximize, nullptr, &rectMax);
    SDL_RenderCopy(renderer, hoverClose ? texCloseActive : texClose, nullptr, &rectClose);
}

void MainTabBar::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        updateMousePosition(event.motion.x, event.motion.y);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int winW;
        SDL_GetWindowSize(SDL_GetWindowFromID(event.button.windowID), &winW, nullptr);

        if (event.button.x >= winW - 30) {
            PostQuitMessage(0); // Cerrar
        }
        else if (event.button.x >= winW - 60 && event.button.x < winW - 30) {
            ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
        }
        else if (event.button.x >= winW - 90 && event.button.x < winW - 60) {
            ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
        }
        else {
            for (size_t i = 0; i < tabs.size(); ++i) {
                SDL_Rect tabRect = getTabRect(static_cast<int>(i));
                if (event.button.x >= tabRect.x && event.button.x <= tabRect.x + tabRect.w &&
                    event.button.y <= tabRect.y + tabRect.h) {

                    if (i != 0 && event.button.x >= tabRect.x + tabRect.w - 15) {
                        closeTab(static_cast<int>(i));
                        return;
                    }
                    else {
                        activeTabIndex = static_cast<int>(i);
                        return;
                    }
                }
            }
        }
    }
}

void MainTabBar::updateMousePosition(int x, int y) {
    mouseX = x;
    mouseY = y;
}

void MainTabBar::addTab(const std::string& title) {
    tabs.push_back({ title, false });
    activeTabIndex = static_cast<int>(tabs.size()) - 1;
}

void MainTabBar::closeTab(int index) {
    if (index == 0 || index >= static_cast<int>(tabs.size())) return;
    tabs.erase(tabs.begin() + index);
    if (activeTabIndex >= index) {
        activeTabIndex = MaxInt(0, static_cast<int>(activeTabIndex - 1));
    }
}
