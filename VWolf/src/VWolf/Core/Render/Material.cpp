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
        inputs = shader->GetMaterialInputs();
        size = shader->GetMaterialSize();

        name = std::string(shader->GetName());
        for (auto input: inputs) {
            switch (input->GetType()) {
                case ShaderDataType::Float4:
                    colors[input->GetName()] = Color();
                    break;
                case ShaderDataType::Float3:
                    vectors[input->GetName()] = Vector3Float();
                    break;
                case ShaderDataType::Float:
                    floats[input->GetName()] = *(new float);
                    break;
                default: break;
            }
            properties[input->GetName()] = input->GetType();
        }
    }

    Material::~Material() {
        
    }

    std::string Material::GetName() {
        return name;
    }

    void Material::SetColor(std::string name, Color color) {
        colors[name] = color;
    }

    void Material::SetVector3(std::string name, Vector3Float vector) {
        vectors[name] = vector;
    }

    void Material::SetFloat(std::string name, float floatNumber) {
        floats[name] = floatNumber;
    }

    Color& Material::GetColor(std::string name) {
        return colors[name];
    }

    Vector3Float& Material::GetVector3(std::string name) {
        return vectors[name];
    }

    float& Material::GetFloat(std::string name) {
        return floats[name];
    }

    std::map<std::string, ShaderDataType> Material::GetProperties() {
        return properties;
    }

    void * Material::GetDataPointer() const {
        char * pointer = (char *)malloc(size);
        memset(pointer, 0, size);
        for (auto input: inputs) {
            switch(input->GetType()) {
                case VWolf::ShaderDataType::Float4:
                    {
                        char * newP = pointer + input->GetOffset();
                        memcpy(newP, value_ptr(colors.find(input->GetName())->second), input->GetSize());
                    }
                    break;
                case VWolf::ShaderDataType::Float3:
                    {
                        char * newP = pointer + input->GetOffset();
                        memcpy(newP, value_ptr(vectors.find(input->GetName())->second), input->GetSize());
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
}
