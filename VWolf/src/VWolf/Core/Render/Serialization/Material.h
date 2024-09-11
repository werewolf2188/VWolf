//
//  Material.h
//  VWolf
//
//  Created by Enrique Ricalde on 4/2/24.
//

#pragma once

enum class MaterialObjectsConstantKeys {
    Material, MaterialName, ShaderName, Colors, Vectors, Floats, PropertyKey, PropertyValue
};

static std::map<MaterialObjectsConstantKeys, const char*> materialKeys = {
    { MaterialObjectsConstantKeys::Material, "Material" },
    { MaterialObjectsConstantKeys::MaterialName, "Name" },
    { MaterialObjectsConstantKeys::ShaderName, "ShaderName" },
    { MaterialObjectsConstantKeys::Colors, "Colors" },
    { MaterialObjectsConstantKeys::Vectors, "Vectors" },
    { MaterialObjectsConstantKeys::Floats, "Floats" },
    { MaterialObjectsConstantKeys::PropertyKey, "Name" },
    { MaterialObjectsConstantKeys::PropertyValue, "Value" }
};

namespace YAML {
    template<>
    struct convert<VWolf::Material>
    {
        static bool decode(const Node& node, VWolf::Material& rhs)
        {
            if (!node.IsMap() ||
                !node[materialKeys[MaterialObjectsConstantKeys::MaterialName]])
                return false;
    
            std::string name = node[materialKeys[MaterialObjectsConstantKeys::MaterialName]].as<std::string>();
            std::string shaderName = node[materialKeys[MaterialObjectsConstantKeys::ShaderName]].as<std::string>();

            rhs.Load(name, shaderName);

            if (node[materialKeys[MaterialObjectsConstantKeys::Colors]]) {
                for (auto kv: node[materialKeys[MaterialObjectsConstantKeys::Colors]]) {
                    std::string key = kv[materialKeys[MaterialObjectsConstantKeys::PropertyKey]].as<std::string>();
                    VWolf::Color value = kv[materialKeys[MaterialObjectsConstantKeys::PropertyValue]].as<VWolf::Color>();
                    rhs.SetColor(key, value);
                }
            }

            if (node[materialKeys[MaterialObjectsConstantKeys::Vectors]]) {
                for (auto kv: node[materialKeys[MaterialObjectsConstantKeys::Vectors]]) {
                    std::string key = kv[materialKeys[MaterialObjectsConstantKeys::PropertyKey]].as<std::string>();
                    VWolf::Vector3 value = kv[materialKeys[MaterialObjectsConstantKeys::PropertyValue]].as<VWolf::Vector3>();
                    rhs.SetVector3(key, value);
                }
            }

            if (node[materialKeys[MaterialObjectsConstantKeys::Floats]]) {
                for (auto kv: node[materialKeys[MaterialObjectsConstantKeys::Floats]]) {
                    std::string key = kv[materialKeys[MaterialObjectsConstantKeys::PropertyKey]].as<std::string>();
                    float value = kv[materialKeys[MaterialObjectsConstantKeys::PropertyValue]].as<float>();
                    rhs.SetFloat(key, value);
                }
            }

            return true;
        }
    };
}

namespace VWolf {
    YAML::Emitter& operator<<(YAML::Emitter& out, VWolf::Material& v)
    {
        out << YAML::BeginMap;
        out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::Material];
        out << YAML::BeginMap;
        out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::MaterialName] << YAML::Value << v.GetName();
        out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::ShaderName] << YAML::Value << v.GetShaderName();
        out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::Colors];
        out << YAML::BeginSeq;

        for (auto& color: v.GetColors()) {
            out << YAML::BeginMap;
            out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::PropertyKey] << YAML::Value << color.first;
            out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::PropertyValue] << YAML::Value << color.second;
            out << YAML::EndMap;
        }
        
        out << YAML::EndSeq;
        out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::Vectors];
        out << YAML::BeginSeq;

        for (auto& vector: v.GetVectors()) {
            out << YAML::BeginMap;
            out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::PropertyKey] << YAML::Value << vector.first;
            out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::PropertyValue] << YAML::Value << vector.second;
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;
        out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::Floats];
        out << YAML::BeginSeq;

        for (auto& floatKV: v.GetFloats()) {
            out << YAML::BeginMap;
            out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::PropertyKey] << YAML::Value << floatKV.first;
            out << YAML::Key << materialKeys[MaterialObjectsConstantKeys::PropertyValue] << YAML::Value << floatKV.second;
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;
        out << YAML::EndMap;
        return out;
    }
}
