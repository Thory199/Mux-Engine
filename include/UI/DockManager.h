#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <vector>

// Forward declarations
class Panel;
class DockGroup;

class DockManager {
public:
    // ===== API ESTÁTICA (para poder llamar DockManager::Xxx() desde Me_Main.cpp) =====
    static void initialize();                          // usa área por defecto 1280x720
    static void initialize(SDL_Window* window);        // obtiene tamaño desde la ventana
    static void initialize(const SDL_Rect& screenArea);// usa un rectángulo dado
    static DockManager* get();                         // devuelve la instancia (o nullptr)
    static void shutdown();                            // destruye la instancia

    // Wrappers estáticos hacia la instancia
    static void render(SDL_Renderer* r, TTF_Font* f);
    static void handleEvent(const SDL_Event& e);
    static void layoutColumns();
    static void setScreenArea(const SDL_Rect& screenArea);
    static SDL_Rect getScreenArea();
    static std::shared_ptr<DockGroup> addGroup(const SDL_Rect& area);
    static void addPanelToAnyGroup(const std::shared_ptr<Panel>& p);

    // ===== API DE INSTANCIA (opcional si quieres usar objetos normales) =====
    explicit DockManager(const SDL_Rect& screenArea);

    // Grupos
    std::shared_ptr<DockGroup> addGroupInst(const SDL_Rect& area);
    const std::vector<std::shared_ptr<DockGroup>>& groups() const;
    std::vector<std::shared_ptr<DockGroup>>& groups();

    // Paneles
    void addPanelToAnyGroupInst(const std::shared_ptr<Panel>& p);

    // Ciclo
    void layoutColumnsInst();                // reparte por columnas el ancho disponible
    void handleEventInst(const SDL_Event& e);
    void renderInst(SDL_Renderer* r, TTF_Font* f);

    // Utilidad
    std::shared_ptr<Panel> makePanel(int x, int y, int w, int h, const char* title);

    // Cambiar área global (si cambia el tamaño de ventana)
    void setScreenAreaInst(const SDL_Rect& screenArea);
    SDL_Rect getScreenAreaInst() const;

private:
    SDL_Rect m_screen{ 0, 0, 1280, 720 };
    std::vector<std::shared_ptr<DockGroup>> m_groups;

    void applyColumnBounds();

    // Singleton
    static std::unique_ptr<DockManager> s_instance;
};
