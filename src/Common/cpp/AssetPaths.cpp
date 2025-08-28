#include <string>
#include <string_view>
#include "AssetPaths.h"

namespace Paths {

    std::string Join(std::string_view base, std::string_view name) {
        auto ends_with_slash = [](std::string_view s) {
            return !s.empty() && (s.back() == '/' || s.back() == '\\');
            };

        std::string out;
        out.reserve(base.size() + name.size() + 1);
        out.append(base);
        if (!ends_with_slash(base)) out.push_back('/');
        out.append(name);
        return out;
    }

} // namespace Paths

