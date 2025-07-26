#include "RTXLoader.h"
#include <fstream>
#include <iostream>

struct RTXHeader {
    uint32_t width;
    uint32_t height;
    uint32_t format; // Esperamos 4 = RGBA8888
};

SDL_Texture* cargarRTX(SDL_Renderer* renderer, const std::string& ruta) {
    std::ifstream in(ruta, std::ios::binary);
    if (!in) {
        std::cerr << "Error abriendo archivo RTX: " << ruta << std::endl;
        return nullptr;
    }

    RTXHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.format != 4) {
        std::cerr << "Formato RTX no soportado: " << header.format << std::endl;
        return nullptr;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, header.width, header.height, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        std::cerr << "Error creando surface: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    std::streamsize imageSize = static_cast<std::streamsize>(static_cast<size_t>(header.width) * header.height * 4);
    in.read(reinterpret_cast<char*>(surface->pixels), imageSize);


    in.close();

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}


