#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class SceneManager {
public:
    static void initialize(SDL_Renderer* renderer);
    static void update(float dt);
    static void render(SDL_Renderer* renderer, TTF_Font* font);
    static void handleEvent(const SDL_Event& e);
    static void shutdown();
};
