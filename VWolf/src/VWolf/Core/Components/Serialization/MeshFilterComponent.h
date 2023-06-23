//
//  MeshFilterRendererComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 6/22/23.
//

#pragma once

enum class MeshFilterComponentKeys {
    Path
};

static std::map<MeshFilterComponentKeys, const char*> meshFilterKeys = {
    { MeshFilterComponentKeys::Path, "Path" }
};

namespace YAML {
    template<>
    struct convert<VWolf::MeshFilterComponent>
    {
        static bool decode(const Node& node, VWolf::MeshFilterComponent& rhs)
        {
            if (!node.IsMap())
                return false;

            std::filesystem::path path(node[meshFilterKeys[MeshFilterComponentKeys::Path]].as<std::string>());
            rhs.SetPath(path);

            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::MeshFilterComponent& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << meshFilterKeys[MeshFilterComponentKeys::Path] << YAML::Value << v.GetPath();
        out << YAML::EndMap;
        return out;
    }
}
