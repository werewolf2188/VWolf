//
//  Extensions.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/16/24.
//

#include "Extensions.h"

#include <algorithm>

namespace VWolfPup {
    Extension::Extension(std::string name, std::string extension): name(name), extension(extension) {}

    Extension::Extension(std::string extension): name(extension), extension(extension) {}

    Extension::Extension(std::filesystem::path path): Extension(std::filesystem::is_directory(path) ? "" : path.extension().string()) {}

    Extension::Extension(const Extension& extension) {
        this->name = extension.name;
        this->extension = extension.extension;
    }

    Extension::Extension(Extension&& extension) {
        this->name = extension.name;
        this->extension = extension.extension;
        
        extension.name = "";
        extension.extension = "";
    }

    Extension& Extension::operator=(const Extension& other) {
        this->name = other.name;
        this->extension = other.extension;
        
        return *this;
    }

    Extension& Extension::operator=(Extension&& other) {
        this->name = other.name;
        this->extension = other.extension;
        
        other.name = "";
        other.extension = "";
        
        return *this;
    }

    bool Extension::operator==(const Extension& other) const {
        return extension == other.extension;
    }

    bool Extension::operator!=(const Extension& other) const {
        return extension != other.extension;
    }

    bool Extension::operator<(const Extension& other) const {
        return other.extension < extension;
    }

    bool operator==(const std::string extension, const Extension& other) {
        return other.extension == extension;
    }

    bool operator==(const Extension& other, const std::string extension) {
        return other.extension == extension;
    }

    bool operator==(const std::filesystem::path path, const Extension& other) {
        if (std::filesystem::is_directory(path)) return false;
        
        return other.extension == path.extension().string();
    }

    bool operator==(const Extension& other, const std::filesystem::path path) {
        if (std::filesystem::is_directory(path)) return false;
        
        return other.extension == path.extension().string();
    }

    const std::string operator+(const std::string name, const Extension& other) {
        return name + other.extension;
    }

    const std::string operator+(const Extension& other, const std::string name) {
        return name + other.extension;
    }

    const std::filesystem::path operator+(const std::filesystem::path path, const Extension& other) {
        return path.string() + other.extension;
    }

    const std::filesystem::path operator+(const Extension& other, const std::filesystem::path path) {
        return path.string() + other.extension;
    }
}
