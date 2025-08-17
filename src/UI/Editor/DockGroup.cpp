#include "DockGroup.h"
#include <algorithm>

DockGroup::DockGroup(const SDL_Rect& bounds)
    : m_bounds(bounds) {
}

void DockGroup::addPanel(const std::shared_ptr<Panel>& p) {
    if (!p) return;

    if (p->rect.w <= 0) p->rect.w = std::max(80, m_bounds.w);
    if (p->rect.h <= 0) p->rect.h = 60;

    if (!rectContains(m_bounds, p->rect)) {
        const int pad = 8;
        p->rect.x = m_bounds.x + pad;
        p->rect.y = m_bounds.y + pad;
        p->rect.w = std::min(p->rect.w, m_bounds.w - pad * 2);
        p->rect.h = std::min(p->rect.h, m_bounds.h - pad * 2);
    }

    m_panels.emplace_back(p);
    layoutVertical();
}

bool DockGroup::removePanel(const std::shared_ptr<Panel>& p) {
    auto it = std::remove_if(m_panels.begin(), m_panels.end(),
        [&](const std::shared_ptr<Panel>& q) { return q.get() == p.get(); });
    bool removed = (it != m_panels.end());
    m_panels.erase(it, m_panels.end());
    if (removed) layoutVertical();
    return removed;
}

void DockGroup::handleEvent(const SDL_Event& e) {
    for (auto it = m_panels.rbegin(); it != m_panels.rend(); ++it) {
        auto& p = *it;
        if (p && p->isVisible()) {
            p->handleEvent(e);
        }
    }
}

void DockGroup::render(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 18, 18, 22, 140);
    SDL_RenderFillRect(renderer, &m_bounds);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderDrawRect(renderer, &m_bounds);

    for (auto& p : m_panels) {
        if (p && p->isVisible()) {
            p->render(renderer, font);
        }
    }
}

void DockGroup::setBounds(const SDL_Rect& r) {
    m_bounds = r;
    layoutVertical();
}

SDL_Rect DockGroup::getBounds() const {
    return m_bounds;
}

std::vector<std::shared_ptr<Panel>>& DockGroup::panels() { return m_panels; }
const std::vector<std::shared_ptr<Panel>>& DockGroup::panels() const { return m_panels; }

bool DockGroup::rectContains(const SDL_Rect& outer, const SDL_Rect& inner) {
    return inner.x >= outer.x && inner.y >= outer.y &&
        inner.x + inner.w <= outer.x + outer.w &&
        inner.y + inner.h <= outer.y + outer.h;
}

void DockGroup::layoutVertical() {
    const int pad = 8;
    int y = m_bounds.y + pad;
    for (auto& p : m_panels) {
        if (!p) continue;
        int h = std::max(p->rect.h, 80);
        p->rect.x = m_bounds.x + pad;
        p->rect.y = y;
        p->rect.w = std::max(80, m_bounds.w - pad * 2);
        p->rect.h = std::min(h, std::max(80, m_bounds.h - (y - m_bounds.y) - pad));
        y += p->rect.h + pad;
        if (y > m_bounds.y + m_bounds.h - pad) break;
    }
}
