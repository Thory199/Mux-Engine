#pragma once
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct MainTab {
    std::string title;
    bool isActive;
};

class MainTabBar {
public:
    static void initialize(SDL_Renderer* renderer);
    static void render(SDL_Renderer* renderer, TTF_Font* font);
    static void handleEvent(const SDL_Event& event);
    static void updateMousePosition(int x, int y); // 🆕 Necesario para botones desde el inicio

    static void addTab(const std::string& title);
    static void closeTab(int index);

private:
    static std::vector<MainTab> tabs;
    static int activeTabIndex;

    static SDL_Rect getTabRect(int index);
    static SDL_Texture* texClose;
    static SDL_Texture* texCloseActive;
    static SDL_Texture* texMinimize;
    static SDL_Texture* texMinimizeActive;
    static SDL_Texture* texMaximize;
    static SDL_Texture* texMaximizeActive;

    static bool hoverClose;
    static bool hoverMinimize;
    static bool hoverMaximize;
};

