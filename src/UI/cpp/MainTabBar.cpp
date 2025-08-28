#include "MainTabBar.h"
#include <algorithm>
#include <SDL2/SDL_image.h>
#include <windows.h>
#include "AssetPaths.h" // rutas centralizadas

inline int MaxInt(int a, int b) { return (a > b) ? a : b; }

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

SDL_Texture* MainTabBar::texTabBG = nullptr;
SDL_Texture* MainTabBar::texTabItem = nullptr; // NUEVO

static int mouseX = 0;
static int mouseY = 0;

static SDL_Texture* LoadSurfaceAsTexture(SDL_Renderer* r, const char* path) {
    SDL_Surface* s = IMG_Load(path);
    if (!s) {
        SDL_Log("IMG_Load fallo: %s (ruta: %s)", IMG_GetError(), path);
        return nullptr;
    }
    SDL_Texture* t = SDL_CreateTextureFromSurface(r, s);
    SDL_FreeSurface(s);
    return t;
}

void MainTabBar::initialize(SDL_Renderer* renderer) {
    tabs.clear();
    tabs.push_back({ "ME1 Project", true });
    activeTabIndex = 0;

    // SOLO cambio: rutas literales -> Paths::InInterface(...)
    texClose = LoadSurfaceAsTexture(renderer, Paths::InInterface("close_idle.png").c_str());
    texCloseActive = LoadSurfaceAsTexture(renderer, Paths::InInterface("close_active.png").c_str());
    texMinimize = LoadSurfaceAsTexture(renderer, Paths::InInterface("minimize_idle.png").c_str());
    texMinimizeActive = LoadSurfaceAsTexture(renderer, Paths::InInterface("minimize_active.png").c_str());
    texMaximize = LoadSurfaceAsTexture(renderer, Paths::InInterface("maximize_idle.png").c_str());
    texMaximizeActive = LoadSurfaceAsTexture(renderer, Paths::InInterface("maximize_active.png").c_str());

    // Fondo de la franja
    texTabBG = IMG_LoadTexture(renderer, Paths::TabBarBG);
    if (!texTabBG) {
        SDL_Log("No se pudo cargar TabBarBG: %s (ruta: %s)", IMG_GetError(), Paths::TabBarBG);
    }

    // NUEVO: “pastilla” de cada pestaña
    texTabItem = IMG_LoadTexture(renderer, Paths::TabItem);
    if (!texTabItem) {
        SDL_Log("No se pudo cargar TabItem: %s (ruta: %s)", IMG_GetError(), Paths::TabItem);
    }
}

SDL_Rect MainTabBar::getTabRect(int index) {
    int x = 40; // margen izquierdo
    for (int i = 0; i < index; ++i) {
        x += static_cast<int>(tabs[i].title.size()) * 8 + 40;
    }
    int width = static_cast<int>(tabs[index].title.size()) * 8 + 30;
    return { x, 0, width, 25 }; // alto 25 px (barra es ~30 px)
}

void MainTabBar::render(SDL_Renderer* renderer, TTF_Font* font) {
    int winW = 0, winH = 0;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    // 1) Fondo de franja (capa inferior)
    SDL_Rect tabBarRect = { 0, 0, winW, 30 };
    if (texTabBG) {
        SDL_RenderCopy(renderer, texTabBG, nullptr, &tabBarRect);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        SDL_RenderFillRect(renderer, &tabBarRect);
    }

    // 2) Pestañas (pastilla + texto + X)
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color gray = { 180, 180, 180, 255 };

    for (size_t i = 0; i < tabs.size(); ++i) {
        SDL_Rect tabRect = getTabRect(static_cast<int>(i));
        tabRect.w += 40;  // alarga 40 px (ajusta a tu gusto)

        // Dibuja la imagen de la pestaña (mismo asset para todas)
        if (texTabItem) {
            SDL_RenderCopy(renderer, texTabItem, nullptr, &tabRect);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 50, 50, 55, 255);
            SDL_RenderFillRect(renderer, &tabRect);
        }

        // Título
        if (font) {
            SDL_Surface* surface = TTF_RenderText_Solid(font, tabs[i].title.c_str(), white);
            if (surface) {
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect textRect = {
                    tabRect.x + (tabRect.w - surface->w) / 2,
                    tabRect.y + (tabRect.h - surface->h) / 2,
                    surface->w, surface->h
                };
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }

        // Botón X (para cerrar, excepto primera pestaña)
        if (i != 0 && font) {
            SDL_Surface* xSurf = TTF_RenderText_Solid(font, "X", gray);
            if (xSurf) {
                SDL_Texture* xTex = SDL_CreateTextureFromSurface(renderer, xSurf);
                SDL_Rect xRect = { tabRect.x + tabRect.w - 15,
                                   tabRect.y + (tabRect.h - xSurf->h) / 2,
                                   xSurf->w, xSurf->h };
                SDL_RenderCopy(renderer, xTex, nullptr, &xRect);
                SDL_FreeSurface(xSurf);
                SDL_DestroyTexture(xTex);
            }
        }
    }

    // 3) Botones ventana (derecha)
    SDL_Rect rectMin = { winW - 90, 0, 30, 25 };
    SDL_Rect rectMax = { winW - 60, 0, 30, 25 };
    SDL_Rect rectClose = { winW - 30, 0, 30, 25 };

    SDL_Point mousePoint = { mouseX, mouseY };
    hoverMinimize = SDL_PointInRect(&mousePoint, &rectMin);
    hoverMaximize = SDL_PointInRect(&mousePoint, &rectMax);
    hoverClose = SDL_PointInRect(&mousePoint, &rectClose);

    if (texMinimize)      SDL_RenderCopy(renderer, hoverMinimize ? texMinimizeActive : texMinimize, nullptr, &rectMin);
    if (texMaximize)      SDL_RenderCopy(renderer, hoverMaximize ? texMaximizeActive : texMaximize, nullptr, &rectMax);
    if (texClose)         SDL_RenderCopy(renderer, hoverClose ? texCloseActive : texClose, nullptr, &rectClose);
}

void MainTabBar::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        updateMousePosition(event.motion.x, event.motion.y);
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int winW;
        SDL_GetWindowSize(SDL_GetWindowFromID(event.button.windowID), &winW, nullptr);

        if (event.button.x >= winW - 30) {
            PostQuitMessage(0);
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

void MainTabBar::updateMousePosition(int x, int y) { mouseX = x; mouseY = y; }

void MainTabBar::addTab(const std::string& title) {
    tabs.push_back({ title, false });
    activeTabIndex = static_cast<int>(tabs.size()) - 1;
}

void MainTabBar::closeTab(int index) {
    if (index == 0 || index >= static_cast<int>(tabs.size())) return;
    tabs.erase(tabs.begin() + index);
    if (activeTabIndex >= index) activeTabIndex = MaxInt(0, activeTabIndex - 1);
}

void MainTabBar::shutdown() {
    auto destroy = [&](SDL_Texture*& t) { if (t) { SDL_DestroyTexture(t); t = nullptr; } };
    destroy(texClose);
    destroy(texCloseActive);
    destroy(texMinimize);
    destroy(texMinimizeActive);
    destroy(texMaximize);
    destroy(texMaximizeActive);
    destroy(texTabBG);
    destroy(texTabItem); // NUEVO
}

void MainTabBar::setFirstTabTitle(const std::string& title) {
    if (!tabs.empty()) {
        tabs[0].title = title;
    }
}



