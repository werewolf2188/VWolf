//
//  Material.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/19/23.
//
#include "vwpch.h"
#include "Material.h"

namespace YAML {
    template<>
    struct convert<VWolf::Material>
    {
        static bool decode(const Node& node, VWolf::Material& rhs)
        {
            rhs.InternalLoad(VWolf::Shader::GetShader(node["shaderName"].as<std::string>()));
            return DeserializeFromBoostDescribe(node, rhs);
        }
    };
}

namespace VWolf {

    Material::Material(const char* shaderName): Material(Shader::GetShader(shaderName)) { }

    Material::Material(std::filesystem::path path) {
        constexpr const char * key = "material";
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(path.string());
        }
        catch (YAML::ParserException e)
        {
            VWOLF_CORE_ERROR("Failed to load .material file '%s'\n     %s", path.string().c_str(), e.what());
        }
        *this = data[key].as<Material>();
    }

    Material::Material(Ref<Shader> shader) {
        name = shader->GetName();
        shaderName = shader->GetName();
        MaterialLibrary::SetMaterial(name.c_str(), this);
        InternalLoad(shader);
    }

    Material::Material(Material& material) {
        this->id = material.id;
        this->name = material.name;
        this->shaderName = material.shaderName;
        this->size = material.size;
        this->colors = material.colors;
        this->vectors = material.vectors;
        this->floats = material.floats;
        this->textures = material.textures;
        this->properties = material.properties;
        this->inputs_information = material.inputs_information;
    }

    Material::Material(Material&& material) {
        this->id = material.id;
        this->name = material.name;
        this->shaderName = material.shaderName;
        this->size = material.size;
        this->colors = material.colors;
        this->vectors = material.vectors;
        this->floats = material.floats;
        this->textures = material.textures;
        this->properties = material.properties;
        this->inputs_information = material.inputs_information;

        material.id = UUID::Empty;
        material.name = std::string();
        material.shaderName = std::string();
        material.size = 0;
        material.colors.clear();
        material.vectors.clear();
        material.floats.clear();
        material.textures.clear();
        material.properties.clear();
        material.inputs_information.clear();
    }

    Material::~Material() {

    }

    void Material::operator=(const Material& material) {
        this->name = material.name;
        this->shaderName = material.shaderName;
        this->size = material.size;
        this->colors = material.colors;
        this->vectors = material.vectors;
        this->floats = material.floats;
        this->textures = material.textures;
        this->properties = material.properties;
        this->inputs_information = material.inputs_information;
    }

    void Material::InternalLoad(Ref<Shader> shader) {
        float floatValue = 0;
        
        size = shader->GetMaterialSize();
        properties = shader->GetSubShader().GetProperties();
        
        for (auto input: shader->GetMaterialInputs()) {
            inputs_information[input->GetName()] = std::make_tuple(input->GetIndex(), input->GetOffset(), input->GetSize());
        }

        for (auto property: properties) {
            switch (property.GetType()) {
                case PropertyType::Color:
                    colors[property.GetName()] = Color(1, 1, 1, 1);
                    break;
                case PropertyType::Vector:
                    vectors[property.GetName()] = Vector4(1, 1, 1, 1);
                    break;
                case PropertyType::Float:
                    floats[property.GetName()] = floatValue;
                    break;
                case PropertyType::Texture2D:
                    textures[property.GetName()] = Texture::LoadTexture2D();
                case PropertyType::Cubemap:
                    textures[property.GetName()] = Texture::LoadCubemap();
                default: break;
            }
        }
    }

    std::string Material::GetName() {
        return name;
    }

    std::string Material::GetShaderName() {
        return shaderName;
    }

    void Material::SetColor(std::string name, Color color) {
        colors[name] = color;
    }

    void Material::SetVector4(std::string name, Vector3 vector) {
        vectors[name] = vector;
    }

    void Material::SetFloat(std::string name, float floatNumber) {
        floats[name] = floatNumber;
    }

    Color& Material::GetColor(std::string name) {
        return colors[name];
    }

    Vector4& Material::GetVector4(std::string name) {
        return vectors[name];
    }

    float& Material::GetFloat(std::string name) {
        return floats[name];
    }

    std::vector<Property> Material::GetProperties() {
        return properties;
    }

    void Material::SetTexture(std::string name, Ref<Texture> texture) {
        textures[name] = texture;
    }

    Ref<Texture> Material::GetTexture(std::string name) {
        return textures[name];
    }

    void * Material::GetDataPointer() const {
        char * pointer = (char *)malloc(size);
        memset(pointer, 0, size);
        for (auto property: properties) {
            auto tuple = (*inputs_information.find(property.GetName())).second;
            char * newP = pointer + std::get<1>(tuple);
            uint32_t sizeP = std::get<2>(tuple);
            switch (property.GetType()) {
                case PropertyType::Color:
                    {
                        const Color& value = colors.find(property.GetName())->second;
                        memcpy(newP, &value, sizeP);
                    }
                    break;
                case PropertyType::Vector:
                    {
                        const Vector4& value = vectors.find(property.GetName())->second;
                        memcpy(newP, &value, sizeP);
                    }
                    break;
                case PropertyType::Float:
                    {
                        memcpy(newP, &floats.find(property.GetName())->second, sizeP);
                    }
                    break;
                default: break;
            }
        }
        return pointer;
    }

    size_t Material::GetSize() const {
        return size;
    }

    void Material::SetAsDefault() {
        MaterialLibrary::SetDefault(this);
    }

    void Material::Save(std::filesystem::path path) {
        YAML::Emitter out;
        out << *this;
        std::ofstream fout(path.string());
        fout << out.c_str();
    }

    Ref<Material> Material::Load(std::filesystem::path path) {
        Material m(path);
        Ref<Material> refM = CreateRef<Material>(m);
        MaterialLibrary::SetMaterial(m.GetName(), refM.get());
        return refM;
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER(Material);

#ifdef VWOLF_CORE
    const std::string defaultKey = "Default";

    std::map<std::string, Material*> MaterialLibrary::materials = {
        { std::string(defaultKey), new Material() }
    };

    Material* MaterialLibrary::GetMaterial(std::string name) {
        return MaterialLibrary::materials[name];
    }

    Material* MaterialLibrary::Default() {
        return MaterialLibrary::GetMaterial(defaultKey);
    }

    void MaterialLibrary::SetDefault(Material* material) {
        SetMaterial(defaultKey, material);
    }

    void MaterialLibrary::SetMaterial(std::string name, Material* material) {
        MaterialLibrary::materials[name] = material;
    }
#endif
}
