#include "DockManager.h"
#include "DockGroup.h"
#include "Panel.h"
#include <algorithm>
#include <memory>

// ======================= Singleton (API estática) =======================
std::unique_ptr<DockManager> DockManager::s_instance{};

void DockManager::initialize() {
    SDL_Rect def{ 0, 0, 1280, 720 };
    s_instance = std::make_unique<DockManager>(def);
}

void DockManager::initialize(SDL_Window* window) {
    int w = 1280, h = 720;
    if (window) SDL_GetWindowSize(window, &w, &h);
    DockManager::initialize(SDL_Rect{ 0, 0, w, h });
}

void DockManager::initialize(const SDL_Rect& screenArea) {
    s_instance = std::make_unique<DockManager>(screenArea);
}

DockManager* DockManager::get() {
    return s_instance.get();
}

void DockManager::shutdown() {
    s_instance.reset();
}

void DockManager::render(SDL_Renderer* r, TTF_Font* f) {
    if (s_instance) s_instance->renderInst(r, f);
}

void DockManager::handleEvent(const SDL_Event& e) {
    if (s_instance) s_instance->handleEventInst(e);
}

void DockManager::layoutColumns() {
    if (s_instance) s_instance->layoutColumnsInst();
}

void DockManager::setScreenArea(const SDL_Rect& screenArea) {
    if (s_instance) s_instance->setScreenAreaInst(screenArea);
}

SDL_Rect DockManager::getScreenArea() {
    return s_instance ? s_instance->getScreenAreaInst() : SDL_Rect{ 0,0,0,0 };
}

std::shared_ptr<DockGroup> DockManager::addGroup(const SDL_Rect& area) {
    return s_instance ? s_instance->addGroupInst(area) : nullptr;
}

void DockManager::addPanelToAnyGroup(const std::shared_ptr<Panel>& p) {
    if (s_instance) s_instance->addPanelToAnyGroupInst(p);
}

// ======================= API de instancia =======================

DockManager::DockManager(const SDL_Rect& screenArea)
    : m_screen(screenArea) {
}

std::shared_ptr<DockGroup> DockManager::addGroupInst(const SDL_Rect& area) {
    auto g = std::make_shared<DockGroup>(area);
    m_groups.emplace_back(g);
    return g;
}

const std::vector<std::shared_ptr<DockGroup>>& DockManager::groups() const { return m_groups; }
std::vector<std::shared_ptr<DockGroup>>& DockManager::groups() { return m_groups; }

void DockManager::addPanelToAnyGroupInst(const std::shared_ptr<Panel>& p) {
    if (m_groups.empty()) {
        auto g = std::make_shared<DockGroup>(m_screen);
        m_groups.emplace_back(g);
    }
    m_groups.front()->addPanel(p);
}

void DockManager::layoutColumnsInst() {
    applyColumnBounds();
}

void DockManager::handleEventInst(const SDL_Event& e) {
    for (auto it = m_groups.rbegin(); it != m_groups.rend(); ++it) {
        (*it)->handleEvent(e);
    }
}

void DockManager::renderInst(SDL_Renderer* r, TTF_Font* f) {
    for (auto& g : m_groups) {
        g->render(r, f);
    }
}

std::shared_ptr<Panel> DockManager::makePanel(int x, int y, int w, int h, const char* title) {
    return std::make_shared<Panel>(x, y, w, h, title);
}

void DockManager::setScreenAreaInst(const SDL_Rect& screenArea) {
    m_screen = screenArea;
    applyColumnBounds();
}

SDL_Rect DockManager::getScreenAreaInst() const {
    return m_screen;
}

void DockManager::applyColumnBounds() {
    if (m_groups.empty()) return;
    int n = static_cast<int>(m_groups.size());
    int colW = std::max(1, m_screen.w / std::max(1, n));
    int x = m_screen.x;

    for (size_t i = 0; i < m_groups.size(); ++i) {
        SDL_Rect b{ x, m_screen.y, colW, m_screen.h };
        if (i == m_groups.size() - 1) {
            b.w = (m_screen.x + m_screen.w) - x; // último hasta el borde
        }
        m_groups[i]->setBounds(b);
        x += colW;
    }
}
