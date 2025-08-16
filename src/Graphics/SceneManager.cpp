#include "SceneManager.h"

void SceneManager::initialize(SDL_Renderer* /*renderer*/) {
    // Carga de escena inicial si hace falta
}

void SceneManager::update(float /*dt*/) {
    // Lógica global de escena (si necesitas)
}

void SceneManager::render(SDL_Renderer* /*renderer*/, TTF_Font* /*font*/) {
    // Render global de escena (si necesitas algo detrás o encima de docks)
}

void SceneManager::handleEvent(const SDL_Event& /*e*/) {
    // Input global de escena (si necesitas)
}

void SceneManager::shutdown() {
    // Liberar recursos de la escena si hace falta
}
