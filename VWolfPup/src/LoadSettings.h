//
//  LoadSettings.hpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 8/13/23.
//

#pragma once
#include <iostream>
#include "VWolf.h"

#define NUMSHADERS 4
#include <array>
#include <map>

namespace VWolfPup {

    class Defaults {
    public:
        Defaults() = default;
        Defaults(Defaults& defaults);
    public:
        inline std::string GetDefaultMaterialName() const { return defaultMaterial; }
        inline std::string GetDefaultSkyBoxMaterialName() const { return defaultSkyBoxMaterial; }
        inline std::string GetDefaultGridMaterialName() const { return defaultGridMaterial; }

        inline void SetDefaultMaterialName(std::string materialName) { defaultMaterial = materialName; }
        inline void SetDefaultSkyBoxMaterialName(std::string materialName) { defaultSkyBoxMaterial = materialName; }
        inline void SetDefaultGridMaterialName(std::string materialName) { defaultGridMaterial = materialName; }
    public:
        static VWolf::Ref<Defaults> Get() { return defaults; }
    public:
        static void Load();
    private:
        std::string defaultMaterial;
        std::string defaultSkyBoxMaterial;
        std::string defaultGridMaterial;
    private:
        static VWolf::Ref<Defaults> defaults;
        
        BOOST_DESCRIBE_CLASS(Defaults, (), (), (), (defaultMaterial, defaultSkyBoxMaterial, defaultGridMaterial))
        VWOLF_SERIALIZATION_FRIENDS(Defaults)
    };
}

namespace YAML {
template<>
    struct convert<VWolfPup::Defaults>
    {
        static bool decode(const Node& node, VWolfPup::Defaults& rhs)
        {
            return VWolf::DeserializeFromBoostDescribe(node, rhs);
        }
    };
}
