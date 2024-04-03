//
//  MaterialSerializer.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/2/24.
//

#include "vwpch.h"
#include "MaterialSerializer.h"

#include "yaml-cpp/yaml.h"

#include "VWolf/Core/Math/Serialization/VMath.h"
#include "Serialization/Material.h"

namespace VWolf {
    void MaterialSerializer::Serialize(Material& material, std::filesystem::path path) {
        YAML::Emitter out;
        out << material;
        std::ofstream fout(path.string());
        fout << out.c_str();
    }

    Ref<Material> MaterialSerializer::Deserialize(std::filesystem::path path) {
        constexpr const char * key = "Material";
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CORE_ERROR("Failed to load .material file '%s'\n     %s", path.string().c_str(), e.what());
        }
        Material m = data[key].as<Material>();
        Ref<Material> refM = CreateRef<Material>(m);
        MaterialLibrary::SetMaterial(m.GetName(), refM.get());
        return refM;
    }
}
