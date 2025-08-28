#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <filesystem>
#include "DockManager.h"
#include "MainTabBar.h"
#include "AssetPaths.h"
#include "Language.h"

SDL_Window* gWindow = nullptr;
const int WIDTH = 1280;
const int HEIGHT = 720;

// --- Estado de menús (Windows -> Developer Tools -> Output Log) ---
static bool gMenuWindowsOpen = false;
static bool gDevToolsOpen = false;

static SDL_Rect gMenuWindowsHit{ 0,0,0,0 }; // zona clicable del label "Windows"
static SDL_Rect gDevToolsItemRect{ 0,0,0,0 }; // item "Developer Tools" del dropdown Windows
static SDL_Rect gDevToolsMenuRect{ 0,0,0,0 }; // panel del submenú Developer Tools
static SDL_Rect gOutputLogItemRect{ 0,0,0,0 }; // item "Output Log" del submenú

void RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, white);
    if (!surface) return;
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

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

    gWindow = SDL_CreateWindow("",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!gWindow) { std::cerr << "Error al crear ventana: " << SDL_GetError() << std::endl; SDL_Quit(); return -1; }

    SDL_SetWindowBordered(gWindow, SDL_FALSE);

    // Icono de ventana (tu BMP)
    SDL_Surface* icon = SDL_LoadBMP(Paths::InInterface("ComingViewIcon.bmp").c_str());
    if (icon) { SDL_SetWindowIcon(gWindow, icon); SDL_FreeSurface(icon); }

    SDL_Renderer* renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) { std::cerr << "Error al crear renderer: " << SDL_GetError() << std::endl; SDL_DestroyWindow(gWindow); SDL_Quit(); return -1; }

    // Fuente
    TTF_Font* font = TTF_OpenFont(Paths::InFonts("Roboto-Regular.ttf").c_str(), 16);
    if (!font) { std::cerr << "Error al cargar fuente: " << TTF_GetError() << std::endl; SDL_DestroyRenderer(renderer); SDL_DestroyWindow(gWindow); SDL_Quit(); return -1; }

    namespace fs = std::filesystem;

    // Idioma: en_US por defecto, luego es_ES, luego primero que exista
    bool loaded = ME1::Lang::Load(Paths::InLanguage("en_US.lang"));
    if (!loaded) loaded = ME1::Lang::Load(Paths::InLanguage("es_ES.lang"));
    if (!loaded) {
        try {
            const std::string langDir = Paths::LanguageRoot;
            if (fs::exists(langDir) && fs::is_directory(langDir)) {
                for (const auto& e : fs::directory_iterator(langDir)) {
                    if (e.is_regular_file() && e.path().extension() == ".lang") {
                        if (ME1::Lang::Load(e.path().string())) { loaded = true; break; }
                    }
                }
            }
        }
        catch (...) {}
    }
    SDL_SetWindowTitle(gWindow, ME1::Lang::T("ui.title").c_str());

    // Logo PNG
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

            // --- MENÚ WINDOWS / DEV TOOLS / OUTPUT LOG ---
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                const int cx = event.button.x;
                const int cy = event.button.y;

                const bool inWindows = (cx >= gMenuWindowsHit.x && cx < (gMenuWindowsHit.x + gMenuWindowsHit.w) &&
                    cy >= gMenuWindowsHit.y && cy < (gMenuWindowsHit.y + gMenuWindowsHit.h));

                if (inWindows) {
                    // Toggle del menú Windows
                    gMenuWindowsOpen = !gMenuWindowsOpen;
                    if (!gMenuWindowsOpen) gDevToolsOpen = false; // si cierras Windows, cierra submenú
                }
                else {
                    if (gMenuWindowsOpen) {
                        const bool inDevItem = (cx >= gDevToolsItemRect.x && cx < (gDevToolsItemRect.x + gDevToolsItemRect.w) &&
                            cy >= gDevToolsItemRect.y && cy < (gDevToolsItemRect.y + gDevToolsItemRect.h));
                        const bool inDevMenu = (cx >= gDevToolsMenuRect.x && cx < (gDevToolsMenuRect.x + gDevToolsMenuRect.w) &&
                            cy >= gDevToolsMenuRect.y && cy < (gDevToolsMenuRect.y + gDevToolsMenuRect.h));

                        if (inDevItem) {
                            // Abrir/cerrar submenú Developer Tools
                            gDevToolsOpen = !gDevToolsOpen;
                        }
                        else if (gDevToolsOpen && inDevMenu) {
                            // Click dentro del submenú
                            const bool inOutputLog = (cx >= gOutputLogItemRect.x && cx < (gOutputLogItemRect.x + gOutputLogItemRect.w) &&
                                cy >= gOutputLogItemRect.y && cy < (gOutputLogItemRect.y + gOutputLogItemRect.h));
                            if (inOutputLog) {
                                // Crear pestaña "Output Log" (vacía por ahora)
                                MainTabBar::addTab("Output Log");
                                // cerrar menús
                                gDevToolsOpen = false;
                                gMenuWindowsOpen = false;
                            }
                        }
                        else {
                            // Click fuera: cerrar menús
                            gDevToolsOpen = false;
                            gMenuWindowsOpen = false;
                        }
                    }
                }
            }

            // --- Tu lógica existente de botones de ventana ---
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

        // 1) Barra de pestañas
        MainTabBar::render(renderer, font);

        // 2) Logo
        if (gLogoTex) {
            int tw = 0, th = 0; SDL_QueryTexture(gLogoTex, nullptr, nullptr, &tw, &th);
            const int targetH = 24;
            int targetW = (tw > 0 && th > 0) ? (tw * targetH) / th : targetH;
            SDL_Rect dstLogo = { 6, (30 - targetH) / 2, targetW, targetH };
            SDL_RenderCopy(renderer, gLogoTex, nullptr, &dstLogo);
        }

        // 3) Barra de menú (File/Edit/Windows/Help)
        SDL_Rect menuBar = { 0, 30, winW, 30 };
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &menuBar);

        // posiciones base de tus labels
        const int xFile = 10;
        const int xEdit = 60;
        const int xWin = 110;
        const int xHelp = 190;
        RenderText(renderer, font, ME1::Lang::T("ui.menu.file").c_str(), xFile, 37);
        RenderText(renderer, font, ME1::Lang::T("ui.menu.edit").c_str(), xEdit, 37);
        RenderText(renderer, font, ME1::Lang::T("ui.menu.window").c_str(), xWin, 37);
        RenderText(renderer, font, ME1::Lang::T("ui.menu.help").c_str(), xHelp, 37);

        // Hitbox de "Windows"
        int winLabelW = 0, winLabelH = 0;
        TTF_SizeText(font, ME1::Lang::T("ui.menu.window").c_str(), &winLabelW, &winLabelH);
        gMenuWindowsHit = { xWin - 4, 30, winLabelW + 8, 30 };

        // 3.1) Dropdown de Windows
        if (gMenuWindowsOpen) {
            const int dropX = gMenuWindowsHit.x;
            const int dropY = menuBar.y + menuBar.h; // debajo de la barra
            const int itemH = 24;
            const int dropW = 200;
            const int dropH = itemH; // solo 1 item por ahora

            SDL_Rect dropRect = { dropX, dropY, dropW, dropH };
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderFillRect(renderer, &dropRect);
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
            SDL_RenderDrawRect(renderer, &dropRect);

            // Item "Developer Tools"
            gDevToolsItemRect = dropRect;

            SDL_Color txt = { 230, 230, 230, 255 };
            SDL_Surface* s = TTF_RenderUTF8_Solid(font, ME1::Lang::T("ui.menu.developer_tools").c_str(), txt);
            if (s) {
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                SDL_Rect r = { dropX + 8, dropY + (itemH - s->h) / 2, s->w, s->h };
                SDL_RenderCopy(renderer, t, nullptr, &r);
                SDL_FreeSurface(s);
                SDL_DestroyTexture(t);
            }

            // 3.2) Submenú a la derecha si está abierto
            if (gDevToolsOpen) {
                const int subX = dropX + dropW; // a la derecha del dropdown
                const int subY = dropY;
                const int subItemH = 24;

                // Solo un item ahora: Output Log
                const int subW = 220;
                const int subH = subItemH;

                gDevToolsMenuRect = { subX, subY, subW, subH };

                SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
                SDL_RenderFillRect(renderer, &gDevToolsMenuRect);
                SDL_SetRenderDrawColor(renderer, 85, 85, 85, 255);
                SDL_RenderDrawRect(renderer, &gDevToolsMenuRect);

                // Item: Output Log
                gOutputLogItemRect = { subX, subY, subW, subItemH };

                SDL_Surface* s2 = TTF_RenderUTF8_Solid(font, ME1::Lang::T("ui.menu.output_log").c_str(), txt);
                if (s2) {
                    SDL_Texture* t2 = SDL_CreateTextureFromSurface(renderer, s2);
                    SDL_Rect r2 = { subX + 8, subY + (subItemH - s2->h) / 2, s2->w, s2->h };
                    SDL_RenderCopy(renderer, t2, nullptr, &r2);
                    SDL_FreeSurface(s2);
                    SDL_DestroyTexture(t2);
                }
            }
        }

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


