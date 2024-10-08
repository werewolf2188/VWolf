//
//  Material.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/19/23.
//

#pragma once

#include "Shader.h"
#include "Texture.h"
#include "VWolf/Core/Math/VMath.h"

namespace VWolf {
    class Material {
    public:
        Material() = default;
        Material(const char* shaderName);
        Material(Ref<Shader> shader);
        Material(Material& material);
        Material(Material&& material);

        ~Material();

        void SetColor(std::string name, Color color);
        void SetVector3(std::string name, Vector3 vector);
        void SetFloat(std::string name, float floatNumber);
        void SetTexture(std::string name, Ref<Texture> texture);
        void SetAsDefault();

        Color& GetColor(std::string name);
        Vector3& GetVector3(std::string name);
        float& GetFloat(std::string name);
        Ref<Texture> GetTexture(std::string name);
        std::map<std::string, ShaderDataType> GetProperties();
        std::string GetName();
        std::string GetShaderName();
#ifdef VWOLF_CORE
        void * GetDataPointer() const;
        size_t GetSize() const;
    
        void Load(std::string name, std::string shaderName);
    public:
        inline std::map<std::string, Color> GetColors() { return colors; }
        inline std::map<std::string, Vector3> GetVectors() { return vectors; }
        inline std::map<std::string, float> GetFloats() { return floats; }
#endif
    public:
        void operator=(const Material& material);
    private:
        std::string name;
        std::string shaderName;
        size_t size;
        std::map<std::string, Color> colors;
        std::map<std::string, Vector3> vectors;
        std::map<std::string, float> floats;
        std::map<std::string, ShaderDataType> properties;
        std::map<std::string, Ref<Texture>> textures;
        std::vector<Ref<ShaderInput>> inputs;
    };

#ifdef VWOLF_CORE
    class MaterialLibrary {
    public:
        static Material* GetMaterial(std::string name);
        static Material* Default();
        static void SetDefault(Material *);
        static void SetMaterial(std::string name, Material* material);
    private:
        static std::map<std::string, Material*> materials;
    };
#endif
}

/* Material_hpp */
