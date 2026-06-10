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

    Extension::Extension(std::string extension): name(""), extension(extension) {}

    Extension::Extension(std::filesystem::path path): Extension(path.extension().string()) {}

    Extension::Extension(const Extension& extension) {
        this->name = extension.name;
        this->extension = extension.extension;
    }

    Extension::Extension(Extension& extension) {
        this->name = extension.name;
        this->extension = extension.extension;
    }

    Extension::Extension(Extension&& extension) {
        this->name = extension.name;
        this->extension = extension.extension;
        
        extension.name = "";
        extension.extension = "";
    }

//    Extension::Extension(std::string name, std::filesystem::path path): Extension(name, path.extension().string()) {}

    std::vector<Extension> Extension::extensions = {
        Extension("VWolf Project", ".vwolfproj"),
        Extension("Scene", ".scene"),
        Extension("Material", ".vwolfmat"),
        Extension("Audio", ".mp3")
    };

    std::string Extension::GetExtension(std::string name) {
        return GetExtensionByName(name).GetExtension();
    }
    
    Extension Extension::GetExtensionByName(std::string name) {
        return (*std::find_if(extensions.begin(), extensions.end(), [name](Extension extension) {
                return extension.GetName() == name;
            }));
    }

    Extension Extension::GetExtensionByExtension(std::string ext) {
        return (*std::find_if(extensions.begin(), extensions.end(), [ext](Extension extension) {
                return extension.GetExtension() == ext;
            }));
    }

    bool Extension::HasExtension(std::string ext) {
        return (std::find_if(extensions.begin(), extensions.end(), [ext](Extension extension) {
                return extension.GetExtension() == ext;
            })) != extensions.end();
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

    bool Extension::operator==(const std::string extension) const {
        return this->extension == extension;
    }

    bool Extension::operator==(const std::filesystem::path path) const {
        if (std::filesystem::is_directory(path)) return false;
        
        return this->extension == path.extension().string();
    }

    bool Extension::operator!=(const Extension& other) const {
        return extension != other.extension;
    }
}


