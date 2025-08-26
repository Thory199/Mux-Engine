#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <initializer_list>

class Panel {
public:
    // Constructores tolerantes (para evitar errores de conversión)
    Panel(); // por si haces make_shared<Panel>() sin args
    explicit Panel(const SDL_Rect& r, const std::string& t = "");
    Panel(const SDL_Rect& r, const char* t);
    Panel(int x, int y, int w, int h, const std::string& t = "");
    Panel(int x, int y, int w, int h, const char* t);

    // Acepta {x,y,w,h} y {x,y,w,h,"Title"}
    explicit Panel(std::initializer_list<int> dims);
    Panel(std::initializer_list<int> dims, const char* t);

    virtual ~Panel() = default;

    // Virtual para que los derivados puedan override
    virtual void handleEvent(const SDL_Event& e);
    virtual void render(SDL_Renderer* renderer, TTF_Font* font);

    // Utilidades
    void setMinSize(int w, int h);
    inline void setVisible(bool v) { visible = v; }
    inline bool isVisible() const { return visible; }

public:
    // ---- Campos públicos (compatibles con tu código actual) ----
    SDL_Rect    rect{ 0, 0, 100, 80 };
    std::string title;

    // Estados de resize usados en varios .cpp
    bool resizing{ false };
    int  resizeOffsetX{ 0 };
    int  resizeOffsetY{ 0 };

protected:
    // Arrastre
    bool dragging{ false };
    int  dragOffsetX{ 0 };
    int  dragOffsetY{ 0 };

    // Visibilidad y límites
    bool visible{ true };
    int  minW{ 80 };
    int  minH{ 60 };

    // Tamaño de las zonas de agarre
    static constexpr int kResizeGrip = 8;

    // Helpers
    bool mouseInRect(int x, int y, const SDL_Rect& r) const;
    void clampMinSize();
};
