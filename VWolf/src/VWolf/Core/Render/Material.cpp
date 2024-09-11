//
//  Material.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/19/23.
//
#include "vwpch.h"
#include "Material.h"

namespace VWolf {

    Material::Material(const char* shaderName): Material(ShaderLibrary::GetShader(shaderName)) { }

    Material::Material(Ref<Shader> shader) {
        float floatValue = 0;
        inputs = shader->GetMaterialInputs();
        size = shader->GetMaterialSize();

        name = std::string(shader->GetName());
        shaderName = std::string(shader->GetName()); // TODO: For now
        MaterialLibrary::SetMaterial(name.c_str(), this);
        for (auto input: inputs) {
            switch (input->GetType()) {
                case ShaderDataType::Float4:
                    colors[input->GetName()] = Color(1, 1, 1, 1);
                    break;
                case ShaderDataType::Float3:
                    vectors[input->GetName()] = Vector3(1, 1, 1);
                    break;
                case ShaderDataType::Float:                    
                    floats[input->GetName()] = floatValue;
                    break;
                default: break;
            }
            properties[input->GetName()] = input->GetType();
        }
        for (auto input: shader->GetTextureInputs()) {
            if (input.GetSize() == (int)ShaderSamplerType::Sampler2D)
                textures[input.GetName()] = Texture::LoadTexture2D();
            if (input.GetSize() == (int)ShaderSamplerType::SamplerCube)
                textures[input.GetName()] = Texture::LoadCubemap();
        }
    }

    Material::Material(Material& material) {
        this->name = material.name;
        this->shaderName = material.shaderName;
        this->inputs = material.inputs;
        this->size = material.size;
        this->colors = material.colors;
        this->vectors = material.vectors;
        this->floats = material.floats;
        this->textures = material.textures;
        this->properties = material.properties;
    }

    Material::Material(Material&& material) {
        this->name = material.name;
        this->shaderName = material.shaderName;
        this->inputs = material.inputs;
        this->size = material.size;
        this->colors = material.colors;
        this->vectors = material.vectors;
        this->floats = material.floats;
        this->textures = material.textures;
        this->properties = material.properties;

        material.name = std::string();
        material.shaderName = std::string();
        material.size = 0;
        material.inputs.clear();
        material.inputs.clear();
        material.colors.clear();
        material.vectors.clear();
        material.floats.clear();
        material.textures.clear();
        material.properties.clear();
    }

    Material::~Material() {

    }

    void Material::operator=(const Material& material) {
        this->name = material.name;
        this->shaderName = material.shaderName;
        this->inputs = material.inputs;
        this->size = material.size;
        this->colors = material.colors;
        this->vectors = material.vectors;
        this->floats = material.floats;
        this->textures = material.textures;
        this->properties = material.properties;
    }

    void Material::Load(std::string name, std::string shaderName) {
        float floatValue = 0;
        this->name = name;
        this->shaderName = shaderName;
        Ref<Shader> shader = ShaderLibrary::GetShader(shaderName);
        inputs = shader->GetMaterialInputs();
        size = shader->GetMaterialSize();

        for (auto input: inputs) {
            switch (input->GetType()) {
                case ShaderDataType::Float4:
                    colors[input->GetName()] = Color(1, 1, 1, 1);
                    break;
                case ShaderDataType::Float3:
                    vectors[input->GetName()] = Vector3(1, 1, 1);
                    break;
                case ShaderDataType::Float:
                    floats[input->GetName()] = floatValue;
                    break;
                default: break;
            }
            properties[input->GetName()] = input->GetType();
        }
        for (auto input: shader->GetTextureInputs()) {
            if (input.GetSize() == (int)ShaderSamplerType::Sampler2D)
                textures[input.GetName()] = Texture::LoadTexture2D();
            if (input.GetSize() == (int)ShaderSamplerType::SamplerCube)
                textures[input.GetName()] = Texture::LoadCubemap();
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

    void Material::SetVector3(std::string name, Vector3 vector) {
        vectors[name] = vector;
    }

    void Material::SetFloat(std::string name, float floatNumber) {
        floats[name] = floatNumber;
    }

    Color& Material::GetColor(std::string name) {
        return colors[name];
    }

    Vector3& Material::GetVector3(std::string name) {
        return vectors[name];
    }

    float& Material::GetFloat(std::string name) {
        return floats[name];
    }

    std::map<std::string, ShaderDataType> Material::GetProperties() {
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
        for (auto input: inputs) {
            switch(input->GetType()) {
                case VWolf::ShaderDataType::Float4:
                    {
                        char * newP = pointer + input->GetOffset();
                        const Color& value = colors.find(input->GetName())->second;
                        memcpy(newP, &value, input->GetSize());
                    }
                    break;
                case VWolf::ShaderDataType::Float3:
                    {
                        char * newP = pointer + input->GetOffset();
                        const Vector3& value = vectors.find(input->GetName())->second;
                        memcpy(newP, &value, input->GetSize());
                    }
                    break;
                case VWolf::ShaderDataType::Float:
                    {
                        char * newP = pointer + input->GetOffset();
                        memcpy(newP, &floats.find(input->GetName())->second, input->GetSize());
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
