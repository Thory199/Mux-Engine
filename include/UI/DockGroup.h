#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <vector>
#include "Panel.h"

class DockGroup {
public:
    explicit DockGroup(const SDL_Rect& bounds);

    // Gestión de paneles
    void addPanel(const std::shared_ptr<Panel>& p);
    bool removePanel(const std::shared_ptr<Panel>& p);

    // Ciclo
    void handleEvent(const SDL_Event& e);
    void render(SDL_Renderer* renderer, TTF_Font* font);

    // Bounds del grupo
    void setBounds(const SDL_Rect& r);
    SDL_Rect getBounds() const;

    // Acceso a los paneles (por si quieres manipular desde fuera)
    std::vector<std::shared_ptr<Panel>>& panels();
    const std::vector<std::shared_ptr<Panel>>& panels() const;

private:
    SDL_Rect m_bounds{ 0,0,640,480 };
    std::vector<std::shared_ptr<Panel>> m_panels;

    static bool rectContains(const SDL_Rect& outer, const SDL_Rect& inner);
    void layoutVertical(); // layout simple: apila paneles verticalmente
};
