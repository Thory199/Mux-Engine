// UI/headers/AssetPaths.h
#pragma once

namespace Paths {
    // Carpeta base de interfaces (igual en Debug/Release)
    inline constexpr const char* Interface = "Data/Interface/";

    // Iconos / logos
    inline constexpr const char* LogoPNG = "Data/Interface/ComingViewIcon.png";

    // Fondo de pestañas (renombrado sin Ñ recomendado)
    inline constexpr const char* TabBarBG = "Data/Interface/BARPESTANA.png";
    // Si mantienes la Ñ, usa:
    // inline constexpr const char* TabBarBG = u8"Data/Interface/BARPESTAÑA.png";
}
