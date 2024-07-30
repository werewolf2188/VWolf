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
}


