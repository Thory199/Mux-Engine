#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "DockManager.h"
#include "MainTabBar.h"

// ✅ rutas centralizadas
#include "AssetPaths.h"

SDL_Window* gWindow = nullptr;
const int WIDTH = 1280;
const int HEIGHT = 720;

void RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) { std::cerr << "Error SDL: " << SDL_GetError() << std::endl; return -1; }
    if (TTF_Init() == -1) { std::cerr << "Error SDL_ttf: " << TTF_GetError() << std::endl; SDL_Quit(); return -1; }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { std::cerr << "Error SDL_image: " << IMG_GetError() << std::endl; SDL_Quit(); return -1; }


    // 👉 Añade esto inmediatamente después de SDL_Init
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best"); // best o "linear"

    gWindow = SDL_CreateWindow("",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!gWindow) { std::cerr << "Error al crear ventana: " << SDL_GetError() << std::endl; SDL_Quit(); return -1; }

    SDL_SetWindowBordered(gWindow, SDL_FALSE);

    // Icono de ventana (solo para la barra del SO)
    SDL_Surface* icon = SDL_LoadBMP("Data/Interface/ComingViewIcon.bmp");
    if (icon) { SDL_SetWindowIcon(gWindow, icon); SDL_FreeSurface(icon); }

    SDL_Renderer* renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) { std::cerr << "Error al crear renderer: " << SDL_GetError() << std::endl; SDL_DestroyWindow(gWindow); SDL_Quit(); return -1; }

    TTF_Font* font = TTF_OpenFont("Data/Local/Fonts/Roboto-Regular.ttf", 16);
    if (!font) { std::cerr << "Error al cargar fuente: " << TTF_GetError() << std::endl; SDL_DestroyRenderer(renderer); SDL_DestroyWindow(gWindow); SDL_Quit(); return -1; }

    // Logo UI (PNG con transparencia) usando Paths
    SDL_Texture* gLogoTex = IMG_LoadTexture(renderer, Paths::LogoPNG);
    if (!gLogoTex) { std::cerr << "Error al cargar logo PNG: " << IMG_GetError() << " (ruta: " << Paths::LogoPNG << ")\n"; }

    DockManager::initialize();
    MainTabBar::initialize(renderer);

    bool running = true;
    SDL_Event event;
    bool maximized = false;

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    MainTabBar::updateMousePosition(mx, my);

    while (running) {
        SDL_GetMouseState(&mx, &my);
        MainTabBar::updateMousePosition(mx, my);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int winW; SDL_GetWindowSize(gWindow, &winW, nullptr);
                int mxClick = event.button.x;

                if (mxClick >= winW - 30) {
                    running = false;
                }
                else if (mxClick >= winW - 60 && mxClick < winW - 30) {
                    HWND hwnd = GetActiveWindow();
                    ShowWindow(hwnd, maximized ? SW_RESTORE : SW_MAXIMIZE);
                    maximized = !maximized;
                }
                else if (mxClick >= winW - 90 && mxClick < winW - 60) {
                    ShowWindow(GetActiveWindow(), SW_MINIMIZE);
                }
            }
            MainTabBar::handleEvent(event);
            DockManager::handleEvent(event);
        }

        // --- RENDER ---
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);

        int winW, winH; SDL_GetWindowSize(gWindow, &winW, &winH);

        // 1) Barra de pestañas (fondo + tabs + botones) - CAPA INFERIOR
        MainTabBar::render(renderer, font);

        // 2) Logo por encima de la barra (para que no lo tape el fondo)
        if (gLogoTex) {
            int tw = 0, th = 0; SDL_QueryTexture(gLogoTex, nullptr, nullptr, &tw, &th);
            const int targetH = 24;
            int targetW = (tw > 0 && th > 0) ? (tw * targetH) / th : targetH;
            SDL_Rect dstLogo = { 6, (30 - targetH) / 2, targetW, targetH };
            SDL_RenderCopy(renderer, gLogoTex, nullptr, &dstLogo);
        }

        // 3) Barra de menú
        SDL_Rect menuBar = { 0, 30, winW, 30 };
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &menuBar);
        RenderText(renderer, font, "File", 10, 37);
        RenderText(renderer, font, "Edit", 60, 37);
        RenderText(renderer, font, "Window", 110, 37);
        RenderText(renderer, font, "Help", 190, 37);

        // 4) Contenido (docks)
        DockManager::render(renderer, font);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    MainTabBar::shutdown();
    if (gLogoTex) SDL_DestroyTexture(gLogoTex);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
}

