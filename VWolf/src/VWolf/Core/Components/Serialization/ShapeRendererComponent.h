//
//  ShapeRendererComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/24/23.
//

#pragma once

enum class ShapeRendererComponentKeys {
    MeshData, Material
};

static std::map<ShapeRendererComponentKeys, const char*> shapeRendererKeys = {
    { ShapeRendererComponentKeys::MeshData, "MeshData" },
    { ShapeRendererComponentKeys::Material, "Material" }
};

namespace YAML {
    template<>
    struct convert<VWolf::ShapeRendererComponent>
    {
        static bool decode(const Node& node, VWolf::ShapeRendererComponent& rhs)
        {
            if (!node.IsMap())
                return false;
            
            std::string meshDataName = node[shapeRendererKeys[ShapeRendererComponentKeys::MeshData]].as<std::string>();
            std::string materialName = node[shapeRendererKeys[ShapeRendererComponentKeys::Material]].as<std::string>();
            
            rhs.SetData(VWolf::ShapeHelper::Create(meshDataName.c_str()));
            rhs.SetMaterial(VWolf::MaterialLibrary::GetMaterial(materialName));

            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::ShapeRendererComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << shapeRendererKeys[ShapeRendererComponentKeys::MeshData] << YAML::Value << v.GetData().GetName();
        out << YAML::Key << shapeRendererKeys[ShapeRendererComponentKeys::Material] << YAML::Value << v.GetMaterial().GetName();
        out << YAML::EndMap;
        return out;
    }
}
