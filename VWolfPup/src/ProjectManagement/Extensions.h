//
//  Extensions.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/16/24.
//

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace VWolfPup {
    class Extension {
    public:
        Extension() = default;
        Extension(std::string extension);
        Extension(std::filesystem::path path);
        Extension(std::string name, std::string extension);
//        Extension(std::string name, std::filesystem::path path);
    public:
        inline std::string GetName() { return name; }
        inline std::string GetExtension() { return extension; }
    public:
        bool operator==(const Extension& other) const;
        bool operator==(const std::string extension) const;
        bool operator!=(const Extension& other) const;
    public:
        static Extension GetExtensionByName(std::string name);
        static Extension GetExtensionByExtension(std::string ext);
        static std::string GetExtension(std::string name);
        static bool HasExtension(std::string ext);
    public:
        static inline std::string GetProjectExtension() { return GetExtension("VWolf Project"); }
        static inline std::string GetSceneExtension() { return GetExtension("Scene"); }
        static inline std::string GetMaterialExtension() { return GetExtension("Material"); }
    private:
        std::string name;
        std::string extension;
    private:
        static std::vector<Extension> extensions;
    };
}
