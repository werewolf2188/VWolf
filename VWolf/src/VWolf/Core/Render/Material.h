//
//  Material.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/19/23.
//

#pragma once

#include "Shader.h"
#include "Texture.h"
#include "VWolf/Core/IIdentifiable.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Core/Utils/GenericSerialization.h"

namespace VWolf {
    class Material: public IIdentifiable {
    public:
        Material() = default;
        Material(std::filesystem::path path);
        Material(const char* shaderName);
        Material(Ref<Shader> shader);
        Material(Material& material);
        Material(Material&& material);

        ~Material();
    public:
        std::string GetName();
        std::string GetShaderName();
    public:
        Color& GetColor(std::string name);
        void SetColor(std::string name, Color color);
        Vector4& GetVector4(std::string name);
        void SetVector4(std::string name, Vector3 vector);
        float& GetFloat(std::string name);
        void SetFloat(std::string name, float floatNumber);
        Ref<Texture> GetTexture(std::string name);
        void SetTexture(std::string name, Ref<Texture> texture);
        
        void SetAsDefault();
        std::vector<Property> GetProperties();
    public:
        void Save(std::filesystem::path path);
    public:
        static Ref<Material> Load(std::filesystem::path path);
#ifdef VWOLF_CORE
    public:
        void * GetDataPointer() const;
        size_t GetSize() const;
#endif
    public:
        void operator=(const Material& material);
    private:
        void InternalLoad(Ref<Shader> shader);
    private:
        std::string name;
        std::string shaderName;
        size_t size;
        std::map<std::string, Color> colors;
        std::map<std::string, Vector4> vectors;
        std::map<std::string, float> floats;
        std::map<std::string, Ref<Texture>> textures;
        
        std::map<std::string, std::tuple<uint32_t, uint32_t, uint32_t>> inputs_information;
        std::vector<Property> properties;
        
        BOOST_DESCRIBE_CLASS(Material, (IIdentifiable), (), (id), (name, shaderName, colors, vectors, floats))
        
        VWOLF_SERIALIZATION_FRIENDS(Material)
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
