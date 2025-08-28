// AssetPaths.h
#pragma once

#include <string>
#include <string_view>

namespace Paths {

	// Carpeta base de interfaces (igual en Debug/Release)
	inline constexpr const char* Interface = "Data/Interface/";

	// Iconos / logos
	inline constexpr const char* LogoPNG = "Data/Interface/ComingViewIcon.png";

	// Fondo de pestañas (renombrado sin Ñ recomendado)
	inline constexpr const char* TabBarBG = "Data/Interface/BARPESTANA.png";
	// Si mantienes la Ñ, usa:
	// inline constexpr const char* TabBarBG = u8"Data/Interface/BARPESTAÑA.png";

	inline constexpr const char* TabItem = "Data/Interface/TabItem.png"; // NUEVO

	// --- Rutas reales según tu proyecto ---
	inline constexpr const char* LocaleRoot = "Data/Local/";
	inline constexpr const char* LocalFonts = "Data/Local/Fonts/";

	// Alias por compatibilidad (si en código viejo se usa "FontsRoot")
	inline constexpr const char* FontsRoot = LocalFonts;

	// --- Prototipo: implementación está en src/Common/cpp/AssetPaths.cpp ---
	std::string Join(std::string_view base, std::string_view name);

	// --- Helpers (renombrados para NO colisionar con la constante 'Interface') ---
	inline std::string InInterface(std::string_view name) { return Join(Interface, name); }
	inline std::string InLocale(std::string_view name) { return Join(LocaleRoot, name); }
	inline std::string InFonts(std::string_view name) { return Join(LocalFonts, name); }


	inline constexpr const char* LanguageRoot = "Data/Local/language/";
	inline std::string InLanguage(std::string_view name) { return Join(LanguageRoot, name); }


	// (Opcionales si los necesitas más adelante)
	// inline constexpr const char* AudioRoot    = "Data/Audio/";
	// inline constexpr const char* TexturesRoot = "Data/Textures/";
	// inline std::string InAudio   (std::string_view name) { return Join(AudioRoot,    name); }
	// inline std::string InTextures(std::string_view name) { return Join(TexturesRoot, name); }

} // namespace Paths

