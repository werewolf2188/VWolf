//
//  MeshRendererComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 6/23/23.
//

#pragma once

enum class MeshRendererComponentKeys {
    Material
};

static std::map<MeshRendererComponentKeys, const char*> meshRendererKeys = {
    { MeshRendererComponentKeys::Material, "Material" }
};

namespace YAML {
    template<>
    struct convert<VWolf::MeshRendererComponent>
    {
        static bool decode(const Node& node, VWolf::MeshRendererComponent& rhs)
        {
            if (!node.IsMap())
                return false;
            
            std::string materialName = node[meshRendererKeys[MeshRendererComponentKeys::Material]].as<std::string>();
            
            rhs.SetMaterial(VWolf::MaterialLibrary::GetMaterial(materialName));

            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::MeshRendererComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << meshRendererKeys[MeshRendererComponentKeys::Material] << YAML::Value << v.GetMaterial().GetName();
        out << YAML::EndMap;
        return out;
    }
}

