#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace autodeco {

enum class Theme { Hell, PurpleHell, HeavenIce, DarkMonochrome };

inline constexpr const char* themeName(Theme t) {
    switch (t) {
        case Theme::Hell: return "Hell";
        case Theme::PurpleHell: return "Purple Hell";
        case Theme::HeavenIce: return "Heaven / Ice";
        case Theme::DarkMonochrome: return "Dark Monochrome";
    }
    return "Hell";
}

struct Seed { int id = 0; float x = 0, y = 0; };
struct Decoration {
    int objectID = 0;
    float x = 0, y = 0, scale = 1, rotation = 0;
    int channel = 1; // GD color channel; users can configure 1 and 2 in level settings
    int zLayer = 5, zOrder = 0;
};

inline bool isBlock(int id) {
    if (id >= 1 && id <= 7) return true;
    if (id >= 90 && id <= 96) return true;
    if (id >= 116 && id <= 122) return true;
    if (id >= 207 && id <= 213) return true;
    if (id == 83 || id == 160 || id == 161 || id == 476 || id == 477) return true;
    return false;
}

inline std::uint32_t hash(std::uint32_t x) {
    x ^= x >> 16; x *= 0x7feb352du;
    x ^= x >> 15; x *= 0x846ca68bu;
    return x ^ (x >> 16);
}

inline int clampPercent(int n) { return std::max(0, std::min(n, 100)); }

// Uses strictly decorative / non-solid object IDs, with no gameplay triggers.
inline std::vector<Decoration> generate(const std::vector<Seed>& selected, Theme theme, int intensity = 60,
                                        bool glow = true, bool air = true, std::size_t maxObjects = 750) {
    std::vector<Decoration> out;
    if (maxObjects == 0) return out;
    const int strength = clampPercent(intensity);
    if (strength == 0) return out;

    std::set<std::pair<int,int>> used;
    auto add = [&](int id, float x, float y, float scale, float rotation, int channel, int zLayer, int zOrder) {
        if (out.size() >= maxObjects || !std::isfinite(x) || !std::isfinite(y)) return;
        out.push_back({id,x,y,scale,rotation,channel,zLayer,zOrder});
    };
    for (auto const& seed : selected) {
        if (out.size() + 6 > maxObjects) break;
        if (!isBlock(seed.id) || !std::isfinite(seed.x) || !std::isfinite(seed.y)) continue;
        if (std::abs(seed.x) > 1000000.f || std::abs(seed.y) > 1000000.f) continue;
        int gx = static_cast<int>(std::lround(seed.x / 30.f));
        int gy = static_cast<int>(std::lround(seed.y / 30.f));
        if (!used.emplace(gx,gy).second) continue;
        auto seedHash = hash(static_cast<std::uint32_t>(gx) * 73856093u ^
                             static_cast<std::uint32_t>(gy) * 19349663u ^
                             static_cast<std::uint32_t>(theme) * 83492791u);
        // Theme-driven base textures: no collision / triggers.
        int tex = 1764, accent = 1766, airTex = 1765;
        float angle = static_cast<float>((seedHash % 8) * 45);
        switch (theme) {
            case Theme::Hell:
                tex = 18; accent = 1766; airTex = 1764; break; // decorative spikes, triangle, dot
            case Theme::PurpleHell:
                tex = 1766; accent = 1752; airTex = 1887; break; // triangle, swirl, small glow
            case Theme::HeavenIce:
                tex = 1764; accent = 1768; airTex = 1887; break; // circle, arrow, glow
            case Theme::DarkMonochrome:
                tex = 1767; accent = 1753; airTex = 1765; break; // rectangle, line, cross
        }
        // 1 = theme primary, 2 = theme highlight (set in level color channels).
        add(tex, seed.x, seed.y + 4.f, theme == Theme::Hell ? 0.38f : 0.72f, angle, 1, 5, 2);
        if (strength >= 25)
            add(accent, seed.x + (seedHash % 2 ? 9.f : -9.f), seed.y - 7.f,
                0.54f, static_cast<float>(seedHash % 360), 2, 5, 4);
        if (strength >= 45 && glow)
            add(1887, seed.x, seed.y, 0.65f, 0.f, 2, 3, -3);
        if (strength >= 65 && air && (seedHash % 3 == 0))
            add(airTex, seed.x + 20.f, seed.y + 25.f, 0.4f, 0.f, 2, 3, 0);
        if (strength >= 85 && glow && (seedHash % 2 == 0))
            add(1886, seed.x - 10.f, seed.y - 11.f, 0.22f, 0.f, 2, 3, -4);
    }
    return out;
}

inline std::string toLevelString(const std::vector<Decoration>& objects) {
    // GD object serialization: ID=1, X=2, Y=3, Rotation=6, Editor layer=20,
    // Main color=21, Z layer=24, Z order=25, Scale=32.
    std::string result;
    result.reserve(objects.size() * 90);
    for (auto const& o : objects) {
        if (!o.objectID || !std::isfinite(o.x) || !std::isfinite(o.y)) continue;
        result += "1," + std::to_string(o.objectID)
               + ",2," + std::to_string(o.x)
               + ",3," + std::to_string(o.y)
               + ",6," + std::to_string(o.rotation)
               + ",21," + std::to_string(o.channel)
               + ",24," + std::to_string(o.zLayer)
               + ",25," + std::to_string(o.zOrder)
               + ",32," + std::to_string(o.scale) + ";";
    }
    return result;
}
} // namespace autodeco
