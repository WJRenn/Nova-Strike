#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <system_error>

#if defined(_WIN32)
  #include <windows.h>
#elif defined(__APPLE__)
  #include <mach-o/dyld.h>
  #include <cstdint>
#endif

// Directory containing the running executable, so assets resolve regardless of the
// current working directory (run-from-anywhere). Falls back to CWD on failure.
inline std::filesystem::path getExecutableDir() {
#if defined(_WIN32)
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (len > 0 && len < MAX_PATH)
        return std::filesystem::path(std::wstring(buf, len)).parent_path();
#elif defined(__APPLE__)
    std::uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buf(size, '\0');
    if (size > 0 && _NSGetExecutablePath(buf.data(), &size) == 0) {
        std::error_code ec;
        auto p = std::filesystem::canonical(buf.c_str(), ec);
        if (!ec) return p.parent_path();
    }
#elif defined(__linux__)
    std::error_code ec;
    auto p = std::filesystem::canonical("/proc/self/exe", ec);
    if (!ec) return p.parent_path();
#endif
    return std::filesystem::current_path();
}

class ResourceManager {
public:
    ResourceManager() {
        basePath = getExecutableDir() / "assets";
        std::cout << "Resource base path: " << basePath << std::endl;
    }
    ~ResourceManager() = default;
    
    // Non-copyable
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void setBasePath(const std::filesystem::path& path) {
        basePath = path;
        std::cout << "Resource base path updated: " << basePath << std::endl;
    }

    sf::Texture& getTexture(const std::string& filename) {
        auto it = textures.find(filename);
        if (it != textures.end()) {
            return it->second;
        }

        std::filesystem::path fullPath = basePath / "images" / filename;
        sf::Texture texture;
        if (!texture.loadFromFile(fullPath.string())) {
            throw std::runtime_error("Failed to load texture: " + fullPath.string());
        }
        
        textures[filename] = std::move(texture);
        return textures[filename];
    }

    sf::Font& getFont(const std::string& filename) {
        auto it = fonts.find(filename);
        if (it != fonts.end()) {
            return it->second;
        }

        std::filesystem::path fullPath = basePath / "fonts" / filename;
        sf::Font font;
        if (!font.openFromFile(fullPath.string())) {
            throw std::runtime_error("Failed to load font: " + fullPath.string());
        }
        
        fonts[filename] = std::move(font);
        return fonts[filename];
    }
    
    std::string getBasePath() const {
        return basePath.string();
    }
    
    void clear() {
        textures.clear();
        fonts.clear();
    }

private:
    std::filesystem::path basePath;
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;
};
