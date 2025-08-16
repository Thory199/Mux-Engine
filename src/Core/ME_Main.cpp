#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "DockManager.h"
#include "MainTabBar.h"

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
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "Error SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Error SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    gWindow = SDL_CreateWindow("",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!gWindow) {
        std::cerr << "Error al crear ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_SetWindowBordered(gWindow, SDL_FALSE);

    SDL_Surface* icon = SDL_LoadBMP("ComingViewIcon.bmp");
    if (icon) {
        SDL_SetWindowIcon(gWindow, icon);
        SDL_FreeSurface(icon);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Error al crear renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("Roboto-Regular.ttf", 16);
    if (!font) {
        std::cerr << "Error al cargar fuente: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return -1;
    }

    DockManager::initialize();
    MainTabBar::initialize(renderer);

    bool running = true;
    SDL_Event event;
    bool maximized = false;

    // Mostrar los botones desde el inicio
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    MainTabBar::updateMousePosition(mx, my); // 🆕 Esto fuerza la visibilidad de los botones desde el inicio


    while (running) {
        SDL_GetMouseState(&mx, &my);
        MainTabBar::updateMousePosition(mx, my);  // Actualiza estado hover

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int winW;
                SDL_GetWindowSize(gWindow, &winW, nullptr);
                int mxClick = event.button.x;

                if (mxClick >= winW - 30) {
                    running = false; // Cerrar
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

        // Render
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);

        int winW, winH;
        SDL_GetWindowSize(gWindow, &winW, &winH);

        SDL_Rect topBar = { 0, 0, winW, 30 };
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &topBar);

        MainTabBar::render(renderer, font);

        SDL_Rect menuBar = { 0, 30, winW, 30 };
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &menuBar);

        RenderText(renderer, font, "File", 10, 37);
        RenderText(renderer, font, "Edit", 60, 37);
        RenderText(renderer, font, "Window", 110, 37);
        RenderText(renderer, font, "Help", 190, 37);

        DockManager::render(renderer, font);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    return 0;
}





