#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "DockGroup.h"

class DockManager {
public:
    static void initialize();
    static void handleEvent(const SDL_Event& event);
    static void render(SDL_Renderer* renderer, TTF_Font* font);
    static const std::vector<DockGroup>& getDocks();

    static int getMenuBarHeight();



private:
    static std::vector<DockGroup> docks;
    static std::shared_ptr<Panel> draggedPanel;
    static DockGroup* sourceDock;
    static int mouseX, mouseY;
    static bool isDragging;
    static bool pendingDrag;  // ✅ Añadido para evitar que clic cause drag
};


