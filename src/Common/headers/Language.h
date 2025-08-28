#pragma once
#include <string>
#include <string_view>
#include <unordered_map>

namespace ME1::Lang {

    class Language {
    public:
        static Language& Instance();

        // Carga un archivo .lang (clave = valor). Devuelve true si ok.
        bool LoadFile(std::string_view filepath);

        // Texto por clave; si no existe, devuelve "{clave}".
        const std::string& Text(std::string_view key) const;

        // ¿Existe la clave?
        bool Has(std::string_view key) const;

        // Limpia el diccionario.
        void Clear();

        const std::string& CurrentFile() const { return current_file_; }

    private:
        Language() = default;

        static void Trim(std::string& s);
        static std::string Unescape(const std::string& s);
        static std::string MissingFormat(std::string_view key);

        std::unordered_map<std::string, std::string> dict_;
        std::string current_file_;
    };

    // Atajos
    inline bool Load(std::string_view filepath) { return Language::Instance().LoadFile(filepath); }
    inline const std::string& T(std::string_view key) { return Language::Instance().Text(key); }
    inline bool Has(std::string_view key) { return Language::Instance().Has(key); }

} // namespace ME1::Lang
