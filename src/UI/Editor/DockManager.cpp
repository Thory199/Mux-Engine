// Archivo: DockManager.cpp
#include "DockManager.h"
#include "Canvas2DPanel.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include <memory>

std::vector<DockGroup> DockManager::docks;
std::shared_ptr<Panel> DockManager::draggedPanel = nullptr;
DockGroup* DockManager::sourceDock = nullptr;
int DockManager::mouseX = 0;
int DockManager::mouseY = 0;
bool DockManager::isDragging = false;
bool DockManager::pendingDrag = false;

int DockManager::getMenuBarHeight() {
    return 60; // Bloquea por debajo de la barra de menú (File, Edit, etc.)
}

#include "Canvas2DPanel.h"  // al inicio del .cpp

void DockManager::initialize() {
    docks.clear();

    DockGroup d1;
    d1.rect = { 0, 60, 300, 400 };
    d1.tabs.push_back(std::make_shared<Panel>(Panel{ "Assets" }));
    d1.tabs.push_back(std::make_shared<Panel>(Panel{ "Propiedades" }));
    docks.push_back(d1);

    DockGroup d2;
    d2.rect = { 300, 60, 680, 700 };
    d2.tabs.push_back(std::make_shared<Canvas2DPanel>());   // 👈 AQUÍ el lienzo 2D real
    d2.tabs.push_back(std::make_shared<Panel>(Panel{ "Viewport 3D" }));
    docks.push_back(d2);

    DockGroup d3;
    d3.rect = { 0, 470, 300, 300 };
    d3.tabs.push_back(std::make_shared<Panel>(Panel{ "Contenido" }));
    docks.push_back(d3);
}


void DockManager::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        mouseX = event.motion.x;
        mouseY = event.motion.y;
        if (pendingDrag && draggedPanel) {
            isDragging = true;
            pendingDrag = false;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        for (auto& dock : docks) {
            int mx = event.button.x;
            int my = event.button.y;
            SDL_Rect titleBar = { dock.rect.x, dock.rect.y, dock.rect.w, 25 };
            if (mx >= titleBar.x && mx <= titleBar.x + titleBar.w &&
                my >= titleBar.y && my <= titleBar.y + titleBar.h) {
                int tabX = dock.rect.x + 5;
                for (size_t i = 0; i < dock.tabs.size(); ++i) {
                    int tabW = 10 + static_cast<int>(dock.tabs[i]->title.size()) * 8 + 20;
                    SDL_Rect tabRect = { tabX, dock.rect.y, tabW, 25 };
                    SDL_Rect closeBtn = { tabX + tabW - 20, dock.rect.y + 5, 10, 10 };

                    if (mx >= closeBtn.x && mx <= closeBtn.x + closeBtn.w &&
                        my >= closeBtn.y && my <= closeBtn.y + closeBtn.h) {
                        dock.tabs.erase(dock.tabs.begin() + i);
                        if (dock.tabs.empty()) {
                            docks.erase(std::remove_if(docks.begin(), docks.end(), [&](const DockGroup& d) {
                                return &d == &dock;
                                }), docks.end());
                        }
                        else if (dock.activeTabIndex >= static_cast<int>(dock.tabs.size())) {
                            dock.activeTabIndex = 0;
                        }
                        return;
                    }

                    if (mx >= tabRect.x && mx <= tabRect.x + tabRect.w) {
                        dock.activeTabIndex = static_cast<int>(i);
                        draggedPanel = dock.tabs[i];
                        sourceDock = &dock;
                        pendingDrag = true;
                        return;
                    }
                    tabX += tabW + 5;
                }
            }
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (isDragging && draggedPanel && sourceDock) {
            bool droppedOnDock = false;
            bool droppedOnSourceDock = false;

            for (auto& dock : docks) {
                SDL_Rect fullDock = dock.rect;
                if (mouseX >= fullDock.x && mouseX <= fullDock.x + fullDock.w &&
                    mouseY >= fullDock.y && mouseY <= fullDock.y + fullDock.h) {
                    if (&dock == sourceDock) {
                        droppedOnSourceDock = true;
                    }
                    else {
                        dock.tabs.push_back(draggedPanel);
                        dock.activeTabIndex = static_cast<int>(dock.tabs.size()) - 1;
                        droppedOnDock = true;
                    }
                    break;
                }
            }

            auto& vec = sourceDock->tabs;

            if (!droppedOnSourceDock) {
                vec.erase(std::remove(vec.begin(), vec.end(), draggedPanel), vec.end());

                if (vec.empty()) {
                    docks.erase(std::remove_if(docks.begin(), docks.end(), [&](const DockGroup& d) {
                        return &d == sourceDock;
                        }), docks.end());
                }
                else if (sourceDock->activeTabIndex >= static_cast<int>(vec.size())) {
                    sourceDock->activeTabIndex = 0;
                }
            }

            if (!droppedOnDock && !droppedOnSourceDock) {
                DockGroup newDock;
                int clampedY = std::max(mouseY - 15, getMenuBarHeight());
                newDock.rect = { mouseX - 50, clampedY, 300, 200 };
                newDock.tabs.push_back(draggedPanel);
                newDock.activeTabIndex = 0;
                docks.push_back(newDock);
            }
        }

        draggedPanel = nullptr;
        sourceDock = nullptr;
        isDragging = false;
        pendingDrag = false;
    }

    for (auto& dock : docks) {
        dock.handleEvent(event);
    }
}

void DockManager::render(SDL_Renderer* renderer, TTF_Font* font) {
    for (auto& dock : docks) {
        if (dock.rect.y < getMenuBarHeight()) dock.rect.y = getMenuBarHeight();
        dock.render(renderer, font);
    }

    if (isDragging && draggedPanel) {
        SDL_Rect preview = { mouseX, mouseY, 100, 25 };
        if (preview.y < getMenuBarHeight()) preview.y = getMenuBarHeight();

        SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
        SDL_RenderFillRect(renderer, &preview);


        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Surface* surface = TTF_RenderText_Solid(font, draggedPanel->title.c_str(), white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = { preview.x + 5, preview.y + 4, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

const std::vector<DockGroup>& DockManager::getDocks() {
    return docks;
}



