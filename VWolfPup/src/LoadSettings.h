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
        bool IsDefault(VWolf::Material& material);
    public:
        inline std::string GetDefaultMaterialName() const { return defaultMaterial; }
        inline std::string GetDefaultSkyBoxMaterialName() const { return defaultSkyBoxMaterial; }
        inline std::string GetDefaultGridMaterialName() const { return defaultGridMaterial; }

        inline void SetDefaultMaterialName(std::string materialName) { defaultMaterial = materialName; }
        inline void SetDefaultSkyBoxMaterialName(std::string materialName) { defaultSkyBoxMaterial = materialName; }
        inline void SetDefaultGridMaterialName(std::string materialName) { defaultGridMaterial = materialName; }

        inline VWolf::Ref<VWolf::Material> GetDefaultMaterial() { return materials[defaultMaterial]; }
        inline VWolf::Ref<VWolf::Material> GetDefaultGridMaterial() { return materials[defaultGridMaterial]; }
        inline VWolf::Ref<VWolf::Material> GetDefaultSkyBoxMaterial() { return materials[defaultSkyBoxMaterial]; }
    public:
        static VWolf::Ref<Defaults> Get() { return defaults; }
    public:
        static void Load();
    private:
        void PrepareMaterials();
    private:
        std::string defaultMaterial;
        std::string defaultSkyBoxMaterial;
        std::string defaultGridMaterial;

        std::map<std::string, VWolf::Ref<VWolf::Material>> materials;
    private:
        static VWolf::Ref<Defaults> defaults;
    };

// ----------------------------------------------- //
    void InitialLoad();
}
