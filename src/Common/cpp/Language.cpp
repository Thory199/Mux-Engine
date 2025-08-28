#include "Language.h"

#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace ME1::Lang {

    // --- Utils ---------------------------------------------------------

    void Language::Trim(std::string& s) {
        auto notspace = [](unsigned char c) { return !std::isspace(c); };
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), notspace));
        s.erase(std::find_if(s.rbegin(), s.rend(), notspace).base(), s.end());
    }

    std::string Language::Unescape(const std::string& s) {
        std::string result;
        result.reserve(s.size());
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\\' && i + 1 < s.size()) {
                char n = s[i + 1];
                if (n == 'n') { result.push_back('\n'); ++i; continue; }
                if (n == 't') { result.push_back('\t'); ++i; continue; }
                if (n == '\\') { result.push_back('\\'); ++i; continue; }
            }
            result.push_back(s[i]);
        }
        return result;
    }

    std::string Language::MissingFormat(std::string_view key) {
        std::string s;
        s.reserve(key.size() + 2);
        s.push_back('{');
        s.append(key.begin(), key.end());
        s.push_back('}');
        return s;
    }

    // --- Singleton -----------------------------------------------------

    Language& Language::Instance() {
        static Language g;
        return g;
    }

    // --- API -----------------------------------------------------------

    bool Language::LoadFile(std::string_view filepath) {
        // Evita el "most vexing parse": usa .open() con una variable intermedia.
        const std::string path(filepath);
        std::ifstream f;
        f.open(path, std::ios::in);
        if (!f.is_open()) {
            dict_.clear();
            current_file_.clear();
            return false;
        }

        std::unordered_map<std::string, std::string> tmp;
        std::string line;

        while (std::getline(f, line)) {
            // Quitar BOM UTF-8 si aparece en la primera línea
            if (!line.empty() && static_cast<unsigned char>(line[0]) == 0xEF) {
                if (line.size() >= 3 &&
                    static_cast<unsigned char>(line[1]) == 0xBB &&
                    static_cast<unsigned char>(line[2]) == 0xBF) {
                    line.erase(0, 3);
                }
            }

            std::string s = line;
            Trim(s);
            if (s.empty() || s[0] == '#' || s[0] == ';') {
                continue; // comentario o vacía
            }

            const size_t pos = s.find('=');
            if (pos == std::string::npos) {
                continue; // línea sin '='
            }

            std::string key = s.substr(0, pos);
            std::string value = s.substr(pos + 1);

            Trim(key);
            Trim(value);
            value = Unescape(value);

            if (!key.empty()) {
                tmp[key] = value;
            }
        }

        dict_.swap(tmp);
        current_file_ = path;
        return true;
    }

    const std::string& Language::Text(std::string_view key) const {
        auto it = dict_.find(std::string(key));
        if (it != dict_.end()) {
            return it->second;
        }
        static thread_local std::string missing;
        missing = MissingFormat(key);
        return missing;
    }

    bool Language::Has(std::string_view key) const {
        return dict_.find(std::string(key)) != dict_.end();
    }

    void Language::Clear() {
        dict_.clear();
        current_file_.clear();
    }

} // namespace ME1::Lang
