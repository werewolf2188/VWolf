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
        Extension(const Extension& extension);
        Extension(Extension&& extension);
    public:
        std::string GetExtension() const { return extension; }
        operator std::string() const { return GetExtension(); }
        operator std::filesystem::path() const { return GetExtension(); }
    public:
        Extension& operator=(const Extension& other);
        Extension& operator=(Extension&& other);

        bool operator==(const Extension& other) const;
        bool operator!=(const Extension& other) const;
        bool operator<(const Extension& other) const;
        
        friend bool operator==(const std::string extension, const Extension& other);
        friend bool operator==(const Extension& other, const std::string extension);
        friend bool operator==(const std::filesystem::path path, const Extension& other);
        friend bool operator==(const Extension& other, const std::filesystem::path path);
        
        friend const std::string operator+(const std::string extension, const Extension& other);
        friend const std::string operator+(const Extension& other, const std::string extension);
        friend const std::filesystem::path operator+(const std::filesystem::path path, const Extension& other);
        friend const std::filesystem::path operator+(const Extension& other, const std::filesystem::path path);
    private:
        std::string name;
        std::string extension;
    };
}
